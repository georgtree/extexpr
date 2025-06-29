
#include "extexpr.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <tcl.h>

extern DLLEXPORT int Extexpr_Init(Tcl_Interp *interp) {

    if (Tcl_InitStubs(interp, "9.0", 0) == NULL) {
        return TCL_ERROR;
    }
    /* check the existence of the namespace */
    if (Tcl_Eval(interp, "namespace eval ::tcl::mathfunc {}") != TCL_OK) {
        return TCL_ERROR;
    }
    /* Provide the current package */
    if (Tcl_PkgProvideEx(interp, PACKAGE_NAME, PACKAGE_VERSION, NULL) != TCL_OK) {
        return TCL_ERROR;
    }
    Tcl_CreateObjCommand2(interp, "::tcl::mathfunc::mullsc", (Tcl_ObjCmdProc2 *)MullscCmdProc2, NULL, NULL);
    Tcl_CreateObjCommand2(interp, "::tcl::mathfunc::sumlsc", (Tcl_ObjCmdProc2 *)SumlscCmdProc2, NULL, NULL);
    Tcl_CreateObjCommand2(interp, "::tcl::mathfunc::sublsc", (Tcl_ObjCmdProc2 *)SublscCmdProc2, NULL, NULL);
    Tcl_CreateObjCommand2(interp, "::tcl::mathfunc::divlsc", (Tcl_ObjCmdProc2 *)DivlscCmdProc2, NULL, NULL);
    Tcl_CreateObjCommand2(interp, "::tcl::mathfunc::powlsc", (Tcl_ObjCmdProc2 *)PowlscCmdProc2, NULL, NULL);
    Tcl_CreateObjCommand2(interp, "::tcl::mathfunc::mulll", (Tcl_ObjCmdProc2 *)MulllCmdProc2, NULL, NULL);
    Tcl_CreateObjCommand2(interp, "::tcl::mathfunc::sumll", (Tcl_ObjCmdProc2 *)SumllCmdProc2, NULL, NULL);
    Tcl_CreateObjCommand2(interp, "::tcl::mathfunc::subll", (Tcl_ObjCmdProc2 *)SubllCmdProc2, NULL, NULL);
    Tcl_CreateObjCommand2(interp, "::tcl::mathfunc::divll", (Tcl_ObjCmdProc2 *)DivllCmdProc2, NULL, NULL);
    Tcl_CreateObjCommand2(interp, "::tcl::mathfunc::powll", (Tcl_ObjCmdProc2 *)PowllCmdProc2, NULL, NULL);
    return TCL_OK;
}
enum Operations { SUM, SUB, MUL, DIV, POW };

/* list-scalar operations */
static int MullscCmdProc2(void *clientData, Tcl_Interp *interp, Tcl_Size objc, Tcl_Obj *const objv[]) {
    if (ListScalarProc(MUL, interp, objc, objv) != TCL_OK) {
        return TCL_ERROR;
    }
    return TCL_OK;
}
static int SumlscCmdProc2(void *clientData, Tcl_Interp *interp, Tcl_Size objc, Tcl_Obj *const objv[]) {
    if (ListScalarProc(SUM, interp, objc, objv) != TCL_OK) {
        return TCL_ERROR;
    }
    return TCL_OK;
}
static int SublscCmdProc2(void *clientData, Tcl_Interp *interp, Tcl_Size objc, Tcl_Obj *const objv[]) {
    if (ListScalarProc(SUB, interp, objc, objv) != TCL_OK) {
        return TCL_ERROR;
    }
    return TCL_OK;
}
static int DivlscCmdProc2(void *clientData, Tcl_Interp *interp, Tcl_Size objc, Tcl_Obj *const objv[]) {
    if (ListScalarProc(DIV, interp, objc, objv) != TCL_OK) {
        return TCL_ERROR;
    }
    return TCL_OK;
}
static int PowlscCmdProc2(void *clientData, Tcl_Interp *interp, Tcl_Size objc, Tcl_Obj *const objv[]) {
    if (ListScalarProc(POW, interp, objc, objv) != TCL_OK) {
        return TCL_ERROR;
    }
    return TCL_OK;
}
static int ListScalarProc(int operation, Tcl_Interp *interp, Tcl_Size objc, Tcl_Obj *const objv[]) {
    Tcl_Obj *resultList = Tcl_NewListObj(0, NULL); /* result list */
    double scalar;                                 /* input scalar value */
    double elementValue;                           /* input list element value */
    double resultValue;                            /* result list element value */
    Tcl_Size listLength;                           /* length of the input list */
    Tcl_Obj **elements;                            /* elements of the input list */
    double intpart;

    if (objc != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "list scalar");
        return TCL_ERROR;
    }
    /* get list elements and scalar value from the command arguments */
    if (Tcl_GetDoubleFromObj(interp, objv[2], &scalar) != TCL_OK) {
        return TCL_ERROR;
    }
    if (Tcl_ListObjGetElements(interp, objv[1], &listLength, &elements) != TCL_OK) {
        return TCL_ERROR;
    }
    /* does actual operation on values */
    for (Tcl_Size i = 0; i < listLength; ++i) {
        if (Tcl_GetDoubleFromObj(interp, elements[i], &elementValue) != TCL_OK) {
            return TCL_ERROR;
        }
        switch ((enum Operations)operation) {
        case SUM:
            resultValue = elementValue + scalar;
            break;
        case MUL:
            resultValue = elementValue * scalar;
            break;
        case SUB:
            resultValue = elementValue - scalar;
            break;
        case DIV:
            if (scalar == 0.0) {
                Tcl_SetObjResult(interp, Tcl_NewStringObj("Division by zero", -1));
                return TCL_ERROR;
            }
            resultValue = elementValue / scalar;
            break;
        case POW:
            if ((modf(scalar, &intpart) != 0.0) && (elementValue < 0.0)) {
                Tcl_SetObjResult(interp, Tcl_NewStringObj("Negative base cannot be raised to a non-integer power", -1));
                return TCL_ERROR;
            }
            resultValue = pow(elementValue, scalar);
            break;
        };
        /* append elements to the result list */
        Tcl_ListObjAppendElement(interp, resultList, Tcl_NewDoubleObj(resultValue));
    }
    /* set result of the command to result list we calculated */
    Tcl_SetObjResult(interp, resultList);
    return TCL_OK;
}

/* list-list operations */
static int MulllCmdProc2(void *clientData, Tcl_Interp *interp, Tcl_Size objc, Tcl_Obj *const objv[]) {
    if (ListListProc(MUL, interp, objc, objv) != TCL_OK) {
        return TCL_ERROR;
    }
    return TCL_OK;
}
static int SumllCmdProc2(void *clientData, Tcl_Interp *interp, Tcl_Size objc, Tcl_Obj *const objv[]) {
    if (ListListProc(SUM, interp, objc, objv) != TCL_OK) {
        return TCL_ERROR;
    }
    return TCL_OK;
}
static int SubllCmdProc2(void *clientData, Tcl_Interp *interp, Tcl_Size objc, Tcl_Obj *const objv[]) {
    if (ListListProc(SUB, interp, objc, objv) != TCL_OK) {
        return TCL_ERROR;
    }
    return TCL_OK;
}
static int DivllCmdProc2(void *clientData, Tcl_Interp *interp, Tcl_Size objc, Tcl_Obj *const objv[]) {
    if (ListListProc(DIV, interp, objc, objv) != TCL_OK) {
        return TCL_ERROR;
    }
    return TCL_OK;
}
static int PowllCmdProc2(void *clientData, Tcl_Interp *interp, Tcl_Size objc, Tcl_Obj *const objv[]) {
    if (ListListProc(POW, interp, objc, objv) != TCL_OK) {
        return TCL_ERROR;
    }
    return TCL_OK;
}
static int ListListProc(int operation, Tcl_Interp *interp, Tcl_Size objc, Tcl_Obj *const objv[]) {
    Tcl_Obj *resultList = Tcl_NewListObj(0, NULL); /* result list */
    double elementValue1, elementValue2;           /* first and second input list elements values */
    double resultValue;                            /* result list element value */
    Tcl_Size list1Length, list2Length;             /* lengths of the first and second input lists */
    Tcl_Obj **elements1, **elements2;              /* elements of the first and second input lists */
    double intpart;
    if (objc != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "list list");
        return TCL_ERROR;
    }
    /* get list elements values from the command arguments */
    if (Tcl_ListObjGetElements(interp, objv[1], &list1Length, &elements1) != TCL_OK) {
        return TCL_ERROR;
    }
    if (Tcl_ListObjGetElements(interp, objv[2], &list2Length, &elements2) != TCL_OK) {
        return TCL_ERROR;
    }
    if (list1Length != list2Length) {
        int len = snprintf(NULL, 0, "Lengths of the input lists '%ld' and '%ld' are not equal", list1Length, list2Length);
        char *buffer = (char *)Tcl_Alloc(len + 1);
        snprintf(buffer, len + 1, "Lengths of the input lists '%ld' and '%ld' are not equal", list1Length, list2Length);
        Tcl_SetObjResult(interp, Tcl_NewStringObj(buffer, -1));
        Tcl_Free(buffer);
        return TCL_ERROR;
    }
    /* does actual multiplication of values */
    for (Tcl_Size i = 0; i < list1Length; ++i) {
        if (Tcl_GetDoubleFromObj(interp, elements1[i], &elementValue1) != TCL_OK) {
            return TCL_ERROR;
        }
        if (Tcl_GetDoubleFromObj(interp, elements2[i], &elementValue2) != TCL_OK) {
            return TCL_ERROR;
        }
        switch ((enum Operations)operation) {
        case SUM:
            resultValue = elementValue1 + elementValue2;
            break;
        case MUL:
            resultValue = elementValue1 * elementValue2;
            break;
        case SUB:
            resultValue = elementValue1 - elementValue2;
            break;
        case DIV:
            if (elementValue2 == 0.0) {
                Tcl_SetObjResult(interp, Tcl_NewStringObj("Division by zero", -1));
                return TCL_ERROR;
            }
            resultValue = elementValue1 / elementValue2;
            break;
        case POW:
            if ((modf(elementValue2, &intpart) != 0.0) && (elementValue1 < 0.0)) {
                Tcl_SetObjResult(interp, Tcl_NewStringObj("Negative base cannot be raised to a non-integer power", -1));
                return TCL_ERROR;
            }
            resultValue = pow(elementValue1, elementValue2);
            break;
        };
        /* append elements to the result list */
        Tcl_ListObjAppendElement(interp, resultList, Tcl_NewDoubleObj(resultValue));
    }
    /* set result of the command to result list we calculated */
    Tcl_SetObjResult(interp, resultList);
    return TCL_OK;
}
