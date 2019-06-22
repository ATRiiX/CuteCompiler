from __future__ import print_function

from ctypes import CFUNCTYPE, c_double

import llvmlite.binding as llvm


# All these initializations are required for code generation!
llvm.initialize()
llvm.initialize_native_target()
llvm.initialize_native_asmprinter()  # yes, even this one

llvm_ir = """
   ; ModuleID = "examples/ir_fpadd.py"
   target triple = "unknown-unknown-unknown"
   target datalayout = ""

   define double @"fpadd"(double %".1", double %".2")
   {
   entry:
     %"res" = fadd double %".1", %".2"
     ret double %"res"
   }
   """
llvm_ir2="""
; ModuleID = 'main'
source_file = "main"

define i32 @main() {
entry:
  %0 = alloca i32
  store i32 0, i32* %0
  %1 = alloca i32
  store i32 0, i32* %1
  %2 = alloca i32
  br i1 true, label %then, label %ifcont

then:                                             ; preds = %entry
  store i32 1, i32* %0
  br label %ifcont

ifcont:                                           ; preds = %then, %entry
  %arrayPtr = load i32, i32* %1
  %3 = load i32, i32* %1
  %cmptmp = icmp ult i32 %3, 2
  %4 = icmp ne i1 %cmptmp, false
  br i1 %4, label %forloop, label %forcont

forloop:                                          ; preds = %forloop, %ifcont
  %arrayPtr1 = load i32, i32* %1
  %5 = load i32, i32* %1
  %addtmp = add i32 %5, 1
  store i32 %addtmp, i32* %1
  %arrayPtr2 = load i32, i32* %1
  %6 = load i32, i32* %1
  %cmptmp3 = icmp ult i32 %6, 2
  %7 = icmp ne i1 %cmptmp3, false
  br i1 %7, label %forloop, label %forcont

forcont:                                          ; preds = %forloop, %ifcont
  store i32 0, i32* %2
  %arrayPtr5 = load i32, i32* %2
  %8 = load i32, i32* %2
  %cmptmp6 = icmp ult i32 %8, 3
  %9 = icmp ne i1 %cmptmp6, false
  br i1 %9, label %forloop4, label %forcont11

forloop4:                                         ; preds = %forloop4, %forcont
  %arrayPtr7 = load i32, i32* %2
  %10 = load i32, i32* %2
  %addtmp8 = add i32 %10, 1
  store i32 %addtmp8, i32* %2
  %arrayPtr9 = load i32, i32* %2
  %11 = load i32, i32* %2
  %cmptmp10 = icmp ult i32 %11, 3
  %12 = icmp ne i1 %cmptmp10, false
  br i1 %12, label %forloop4, label %forcont11

forcont11:                                        ; preds = %forloop4, %forcont
  ret i32 0
}

        """
def create_execution_engine():
    """
    Create an ExecutionEngine suitable for JIT code generation on
    the host CPU.  The engine is reusable for an arbitrary number of
    modules.
    """
    # Create a target machine representing the host
    target = llvm.Target.from_default_triple()
    target_machine = target.create_target_machine()
    # And an execution engine with an empty backing module
    backing_mod = llvm.parse_assembly("")
    engine = llvm.create_mcjit_compiler(backing_mod, target_machine)
    return engine


def compile_ir(engine, llvm_ir):
    """
    Compile the LLVM IR string with the given engine.
    The compiled module object is returned.
    """
    # Create a LLVM module object from the IR
    mod = llvm.parse_assembly(llvm_ir)
    mod.verify()
    # Now add the module and make sure it is ready for execution
    engine.add_module(mod)
    engine.finalize_object()
    return mod


engine = create_execution_engine()
mod = compile_ir(engine, llvm_ir)
mod2= compile_ir(engine, llvm_ir2)
#llvm.ModulePassManager(mod)

print(mod2.as_bitcode())
PassManagerBuilder=llvm.PassManagerBuilder()
PassManagerBuilder.opt_level=3
#PassManagerBuilder.opt_level(3)

PassManager=llvm.create_module_pass_manager()
PassManagerBuilder.populate(PassManager)

if llvm.ModulePassManager(PassManager).run(mod2):
    print(mod2.as_bitcode())
# Look up the function pointer (a Python int)
func_ptr = engine.get_function_address("fpadd")

# Run the function via ctypes
cfunc = CFUNCTYPE(c_double, c_double, c_double)(func_ptr)
res = cfunc(1.0, 3.5)
print("fpadd(...) =", res)
