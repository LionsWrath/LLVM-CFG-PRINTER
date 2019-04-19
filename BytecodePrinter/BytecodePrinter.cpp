#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"

#include <string>
#include <vector>
#include <unordered_map>

using namespace llvm;

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
        for (int i=0; i<rshift; i++)
            rs += " ";

        fl << rs << nodeName << " [shape=record," << "\n";
        fl << rs << "    label=\"{BB" << nodeId << "\\l\\l\n";
        
        for (auto& rep : instr) {
            fl << rs << "            " << rep;

            if (rep != *(instr.rbegin()))
                fl << "\n";
        }
        
        if (succs.size() == 1) {
            fl << "\\l}\"];\n";
            fl << rs << nodeName << " -> " << succs[0] << ";\n";
        } else if (succs.size() == 0){ 
            fl << "\\l}\"];\n";
        } else {
            fl << "\\l|{<s0>T|<s1>F}}\"];\n";
        
            for (int i=0; i<succs.size(); i++) {
                fl << rs << nodeName << ":s" << i << " -> " << succs[i] << ";\n";
            }
        }
    }
};

namespace {

struct BytecodePrinter : public FunctionPass {
private:
    std::unordered_map<std::string, unsigned> bbNaming;
    std::vector<DotNode> dotNodes;
    unsigned idCount;

    unsigned addBB(BasicBlock& bb) {
        if (bbNaming.find(bb.getName()) == end(bbNaming)) {
            bbNaming[bb.getName()] = idCount++;
        }

        return bbNaming[bb.getName()];
    }

    unsigned getBBId(const std::string& name) {
        return bbNaming[name];
    }

    bool isBB(const std::string name) {
        return bbNaming.find(name) != end(bbNaming);
    }

    void writeFunctionToFile(Function& F) {
        std::string filename = F.getName().str() + ".dot"; 

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

    void resetPass() {
        this->idCount = 0;
        this->bbNaming.clear();
        this->dotNodes.clear();
    }

public:
    static char ID;

    BytecodePrinter() : FunctionPass(ID), bbNaming(), dotNodes(), idCount(0) {}

    bool runOnFunction(Function &F) override {
        this->resetPass();

        // Iterate over basic blocks
        for (BasicBlock& BB : F) {

            DotNode node(BB.getName(), addBB(BB));

            std::string rep;
            raw_string_ostream rso(rep);

            // Iterate over instructions
            for (Instruction &I : BB) {

                if (I.isTerminator()) {

                    for (unsigned i=0; i<I.getNumSuccessors(); i++) {
                        BasicBlock *succ = I.getSuccessor(i);
                        addBB(*succ);

                        // Add successors to Node
                        node.addSucc(succ->getName());
                    }

                    // Get textual representation
                    rep += I.getOpcodeName();

                    for (int i=0; i<I.getNumOperands(); i++) {

                        auto v = I.getOperand(i);

                        if (v->hasName()) {
                            if (isBB(v->getName().str()))
                                rep += " %BB" + std::to_string(getBBId(v->getName()));
                            else
                                rep += " %" + v->getName().str();
                        }
                    }
                   
                    node.addInstr(rep);
                    rep.clear();
                } else {

                    // Get textual representation
                    rep += I.getOpcodeName();

                    for (int i=0; i<I.getNumOperands(); i++) {

                        auto v = I.getOperand(i);

                        if (isa<Constant>(v))

                        rep += " %" + v->getName().str();
                    }

                    rep += "|";
                    I.print(rso);
                    
                    // Add instruction to Node
                    node.addInstr(rep);
                    rep.clear();
                }
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
