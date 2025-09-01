
#include <tcl.h>

const char *TclGetUnqualifiedName(const char *qualifiedName);
extern DLLEXPORT int extexpr_Init(Tcl_Interp *interp);
static int OperationProc(int operation, Tcl_Interp *interp, Tcl_Size objc, Tcl_Obj *const objv[]);
static int MulCmdProc2(void *clientData, Tcl_Interp *interp, Tcl_Size objc, Tcl_Obj *const objv[]);
static int SumCmdProc2(void *clientData, Tcl_Interp *interp, Tcl_Size objc, Tcl_Obj *const objv[]);
static int SubCmdProc2(void *clientData, Tcl_Interp *interp, Tcl_Size objc, Tcl_Obj *const objv[]);
static int DivCmdProc2(void *clientData, Tcl_Interp *interp, Tcl_Size objc, Tcl_Obj *const objv[]);
static int PowCmdProc2(void *clientData, Tcl_Interp *interp, Tcl_Size objc, Tcl_Obj *const objv[]);
static int DotCmdProc2(void *clientData, Tcl_Interp *interp, Tcl_Size objc, Tcl_Obj *const objv[]);
