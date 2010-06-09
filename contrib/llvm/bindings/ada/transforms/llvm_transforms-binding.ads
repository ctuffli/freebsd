-- This file is generated by SWIG. Do *not* modify by hand.
--

with llvm;


package LLVM_Transforms.Binding is

   procedure LLVMAddArgumentPromotionPass (PM : in llvm.LLVMPassManagerRef);

   procedure LLVMAddConstantMergePass (PM : in llvm.LLVMPassManagerRef);

   procedure LLVMAddDeadArgEliminationPass
     (PM : in llvm.LLVMPassManagerRef);

   procedure LLVMAddDeadTypeEliminationPass
     (PM : in llvm.LLVMPassManagerRef);

   procedure LLVMAddFunctionAttrsPass (PM : in llvm.LLVMPassManagerRef);

   procedure LLVMAddFunctionInliningPass (PM : in llvm.LLVMPassManagerRef);

   procedure LLVMAddGlobalDCEPass (PM : in llvm.LLVMPassManagerRef);

   procedure LLVMAddGlobalOptimizerPass (PM : in llvm.LLVMPassManagerRef);

   procedure LLVMAddIPConstantPropagationPass
     (PM : in llvm.LLVMPassManagerRef);

   procedure LLVMAddLowerSetJmpPass (PM : in llvm.LLVMPassManagerRef);

   procedure LLVMAddPruneEHPass (PM : in llvm.LLVMPassManagerRef);

   procedure LLVMAddRaiseAllocationsPass (PM : in llvm.LLVMPassManagerRef);

   procedure LLVMAddStripDeadPrototypesPass
     (PM : in llvm.LLVMPassManagerRef);

   procedure LLVMAddStripSymbolsPass (PM : in llvm.LLVMPassManagerRef);

   procedure LLVMAddAggressiveDCEPass (PM : in llvm.LLVMPassManagerRef);

   procedure LLVMAddCFGSimplificationPass (PM : in llvm.LLVMPassManagerRef);

   procedure LLVMAddCondPropagationPass (PM : in llvm.LLVMPassManagerRef);

   procedure LLVMAddDeadStoreEliminationPass
     (PM : in llvm.LLVMPassManagerRef);

   procedure LLVMAddGVNPass (PM : in llvm.LLVMPassManagerRef);

   procedure LLVMAddIndVarSimplifyPass (PM : in llvm.LLVMPassManagerRef);

   procedure LLVMAddInstructionCombiningPass
     (PM : in llvm.LLVMPassManagerRef);

   procedure LLVMAddJumpThreadingPass (PM : in llvm.LLVMPassManagerRef);

   procedure LLVMAddLICMPass (PM : in llvm.LLVMPassManagerRef);

   procedure LLVMAddLoopDeletionPass (PM : in llvm.LLVMPassManagerRef);

   procedure LLVMAddLoopIndexSplitPass (PM : in llvm.LLVMPassManagerRef);

   procedure LLVMAddLoopRotatePass (PM : in llvm.LLVMPassManagerRef);

   procedure LLVMAddLoopUnrollPass (PM : in llvm.LLVMPassManagerRef);

   procedure LLVMAddLoopUnswitchPass (PM : in llvm.LLVMPassManagerRef);

   procedure LLVMAddMemCpyOptPass (PM : in llvm.LLVMPassManagerRef);

   procedure LLVMAddPromoteMemoryToRegisterPass
     (PM : in llvm.LLVMPassManagerRef);

   procedure LLVMAddReassociatePass (PM : in llvm.LLVMPassManagerRef);

   procedure LLVMAddSCCPPass (PM : in llvm.LLVMPassManagerRef);

   procedure LLVMAddScalarReplAggregatesPass
     (PM : in llvm.LLVMPassManagerRef);

   procedure LLVMAddSimplifyLibCallsPass (PM : in llvm.LLVMPassManagerRef);

   procedure LLVMAddTailCallEliminationPass
     (PM : in llvm.LLVMPassManagerRef);

   procedure LLVMAddConstantPropagationPass
     (PM : in llvm.LLVMPassManagerRef);

   procedure LLVMAddDemoteMemoryToRegisterPass
     (PM : in llvm.LLVMPassManagerRef);

private

   pragma Import
     (C,
      LLVMAddArgumentPromotionPass,
      "Ada_LLVMAddArgumentPromotionPass");
   pragma Import
     (C,
      LLVMAddConstantMergePass,
      "Ada_LLVMAddConstantMergePass");
   pragma Import
     (C,
      LLVMAddDeadArgEliminationPass,
      "Ada_LLVMAddDeadArgEliminationPass");
   pragma Import
     (C,
      LLVMAddDeadTypeEliminationPass,
      "Ada_LLVMAddDeadTypeEliminationPass");
   pragma Import
     (C,
      LLVMAddFunctionAttrsPass,
      "Ada_LLVMAddFunctionAttrsPass");
   pragma Import
     (C,
      LLVMAddFunctionInliningPass,
      "Ada_LLVMAddFunctionInliningPass");
   pragma Import (C, LLVMAddGlobalDCEPass, "Ada_LLVMAddGlobalDCEPass");
   pragma Import
     (C,
      LLVMAddGlobalOptimizerPass,
      "Ada_LLVMAddGlobalOptimizerPass");
   pragma Import
     (C,
      LLVMAddIPConstantPropagationPass,
      "Ada_LLVMAddIPConstantPropagationPass");
   pragma Import (C, LLVMAddLowerSetJmpPass, "Ada_LLVMAddLowerSetJmpPass");
   pragma Import (C, LLVMAddPruneEHPass, "Ada_LLVMAddPruneEHPass");
   pragma Import
     (C,
      LLVMAddRaiseAllocationsPass,
      "Ada_LLVMAddRaiseAllocationsPass");
   pragma Import
     (C,
      LLVMAddStripDeadPrototypesPass,
      "Ada_LLVMAddStripDeadPrototypesPass");
   pragma Import (C, LLVMAddStripSymbolsPass, "Ada_LLVMAddStripSymbolsPass");
   pragma Import
     (C,
      LLVMAddAggressiveDCEPass,
      "Ada_LLVMAddAggressiveDCEPass");
   pragma Import
     (C,
      LLVMAddCFGSimplificationPass,
      "Ada_LLVMAddCFGSimplificationPass");
   pragma Import
     (C,
      LLVMAddCondPropagationPass,
      "Ada_LLVMAddCondPropagationPass");
   pragma Import
     (C,
      LLVMAddDeadStoreEliminationPass,
      "Ada_LLVMAddDeadStoreEliminationPass");
   pragma Import (C, LLVMAddGVNPass, "Ada_LLVMAddGVNPass");
   pragma Import
     (C,
      LLVMAddIndVarSimplifyPass,
      "Ada_LLVMAddIndVarSimplifyPass");
   pragma Import
     (C,
      LLVMAddInstructionCombiningPass,
      "Ada_LLVMAddInstructionCombiningPass");
   pragma Import
     (C,
      LLVMAddJumpThreadingPass,
      "Ada_LLVMAddJumpThreadingPass");
   pragma Import (C, LLVMAddLICMPass, "Ada_LLVMAddLICMPass");
   pragma Import (C, LLVMAddLoopDeletionPass, "Ada_LLVMAddLoopDeletionPass");
   pragma Import
     (C,
      LLVMAddLoopIndexSplitPass,
      "Ada_LLVMAddLoopIndexSplitPass");
   pragma Import (C, LLVMAddLoopRotatePass, "Ada_LLVMAddLoopRotatePass");
   pragma Import (C, LLVMAddLoopUnrollPass, "Ada_LLVMAddLoopUnrollPass");
   pragma Import (C, LLVMAddLoopUnswitchPass, "Ada_LLVMAddLoopUnswitchPass");
   pragma Import (C, LLVMAddMemCpyOptPass, "Ada_LLVMAddMemCpyOptPass");
   pragma Import
     (C,
      LLVMAddPromoteMemoryToRegisterPass,
      "Ada_LLVMAddPromoteMemoryToRegisterPass");
   pragma Import (C, LLVMAddReassociatePass, "Ada_LLVMAddReassociatePass");
   pragma Import (C, LLVMAddSCCPPass, "Ada_LLVMAddSCCPPass");
   pragma Import
     (C,
      LLVMAddScalarReplAggregatesPass,
      "Ada_LLVMAddScalarReplAggregatesPass");
   pragma Import
     (C,
      LLVMAddSimplifyLibCallsPass,
      "Ada_LLVMAddSimplifyLibCallsPass");
   pragma Import
     (C,
      LLVMAddTailCallEliminationPass,
      "Ada_LLVMAddTailCallEliminationPass");
   pragma Import
     (C,
      LLVMAddConstantPropagationPass,
      "Ada_LLVMAddConstantPropagationPass");
   pragma Import
     (C,
      LLVMAddDemoteMemoryToRegisterPass,
      "Ada_LLVMAddDemoteMemoryToRegisterPass");

end LLVM_Transforms.Binding;
