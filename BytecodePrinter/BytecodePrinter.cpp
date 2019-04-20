#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/CommandLine.h"

#include <string>
#include <vector>
#include <unordered_map>

using namespace llvm;

static cl::opt<std::string> FilenameSuffix("s", 
        cl::desc("Suffix for output dot files"), cl::value_desc("suffix"));

class DotNode {
private:
    unsigned nodeId;
    std::string nodeName;
    std::vector<std::string> instr;
    std::vector<std::string> succs;

public:
    DotNode(std::string name, unsigned id): 
        nodeName(name), nodeId(id), instr(), succs() {}

    void addInstr(std::string rep) {
        instr.push_back(rep);
    }

    void addSucc(std::string name) {
        succs.push_back(name);
    }

    void writeNodeToFile(raw_fd_ostream& fl, unsigned rshift = 0) {
        std::string rs = "";
        for (unsigned i=0; i<rshift; i++)
            rs += " ";

        fl << rs << "\"" << nodeName << "\"" << " [shape=record," << "\n";
        fl << rs << "    label=\"{BB" << nodeId << "\\l\\l\n";
        
        for (auto& rep : instr) {
            fl << rs << "            " << rep << "\\l";

            if (rep != *(instr.rbegin()))
                fl << "\n";
        }
        
        if (succs.size() == 1) {
            fl << "\\l}\"];\n";
            fl << rs << "\"" << nodeName << "\" -> \"" << succs[0] << "\";\n";
        } else if (succs.size() == 0){ 
            fl << "\\l}\"];\n";
        } else {
            fl << "\\l|{<s0>T|<s1>F}}\"];\n";
        
            for (int i=0; i<succs.size(); i++) {
                fl << rs << "\"" << nodeName << "\":s" << i << " -> \"" << succs[i] << "\";\n";
            }
        }
    }
};

namespace {

struct BytecodePrinter : public FunctionPass {
private:
    std::unordered_map<std::string, unsigned int> bbNaming;
    std::vector<DotNode> dotNodes;
    unsigned int idCount;
    unsigned int vCount;

    unsigned int addBB(BasicBlock& bb) {
        if (bbNaming.find(bb.getName()) == end(bbNaming)) {
            bbNaming[bb.getName()] = idCount++;
        }

        return bbNaming[bb.getName()];
    }

    unsigned int getBBId(const std::string& name) {
        return bbNaming[name];
    }

    bool isBB(const std::string name) {
        return bbNaming.find(name) != end(bbNaming);
    }

    void writeFunctionToFile(Function& F) {
        std::string filename = F.getName().str() + ".dot"; 

        if (FilenameSuffix.getNumOccurrences() > 0)
            filename = FilenameSuffix + "_" + filename;

        std::error_code EC;
        raw_fd_ostream fl(filename, EC, sys::fs::OpenFlags::OF_Text);

        if (!EC) {
            fl << "digraph \"CFG for \'" << F.getName().str() << "' function \" {\n";
            
            for (auto& node : dotNodes) {
                node.writeNodeToFile(fl, 4);
            }

            fl << "}\n";
        } else {
            errs() << "ERROR opening file " << filename << "." << "\n";
        }
    }

    std::string getOperandStr(Value* v) {
        std::string rep = "";
        raw_string_ostream rso(rep);

        if (!v->hasName())
            v->setName(std::to_string(vCount++));

        if (isa<BasicBlock>(v)) {
            rep += " BB" + std::to_string(getBBId(v->getName()));
        } else if (isa<ConstantInt>(v)) {
            auto* CI = cast<ConstantInt>(v);
            rep += " " + std::to_string(CI->getSExtValue());
        } else if (isa<ConstantExpr>(v)) {
            auto* CE = cast<ConstantExpr>(v);
            rep += " (";
            for (int j=0; j<CE->getNumOperands(); j++) {
                rep += getOperandStr(CE->getOperand(j));
            }
            rep += " )";
        } else if (isa<ConstantAggregate>(v)) {
            auto* CA = cast<ConstantAggregate>(v);
            rep += " (";
            for (int j=0; j<CA->getNumOperands(); j++) {
                rep += getOperandStr(CA->getOperand(j));
            }
            rep += " )";
        } else if (isa<PHINode>(v)) {
            auto* PN = cast<PHINode>(v);
            rep += " [";
            for (int j=0; j<PN->getNumOperands(); j++) {
                rep += getOperandStr(PN->getOperand(j));
            }
            rep += " ]";
        } else {
            rep += " %" + v->getName().str(); 
        }  

        return rep;
    }

    void resetPass() {
        this->idCount = 0;
        this->vCount = 0;
        this->bbNaming.clear();
        this->dotNodes.clear();
    }

public:
    static char ID;

    BytecodePrinter() : FunctionPass(ID), 
        bbNaming(), dotNodes(), idCount(0), vCount(0) {}

    bool runOnFunction(Function &F) override {
        this->resetPass();

        for (BasicBlock& BB : F) {

            DotNode node(BB.getName(), addBB(BB));
            std::string rep;

            for (Instruction &I : BB) {
                // Name the instruction
                if (!I.hasName() && !I.getType()->isVoidTy())
                    I.setName(std::to_string(vCount++));
                
                // Add the successors
                if (I.isTerminator()) {
                    for (unsigned i=0; i<I.getNumSuccessors(); i++) {
                        BasicBlock *succ = I.getSuccessor(i);
                        addBB(*succ);

                        // Add successors to Node
                        node.addSucc(succ->getName());
                    }
                }

                // Get textual representation
                if (!I.getType()->isVoidTy()) {
                    rep += "%" + I.getName().str() + " = ";
                }

                rep += I.getOpcodeName();
                
                for (unsigned i=0; i<I.getNumOperands(); i++) {
                    auto v = I.getOperand(i);
                    rep += getOperandStr(v);    
                }
                
                // Add instruction to Node
                node.addInstr(rep);
                rep.clear();
            }
            
            dotNodes.push_back(node);
        }

        this->writeFunctionToFile(F);
        
        return false;
    }
};

} /* anonymous namespace end */

char BytecodePrinter::ID = 0;

static RegisterPass<BytecodePrinter> X(
        "bytecode-printer", 
        "Pass to print the CFG",
        false,
        false);
