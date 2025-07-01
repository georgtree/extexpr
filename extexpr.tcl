package provide extexpr 0.1

namespace eval tcl::mathfunc {
    proc llength {list} {
        # Wraps `llength` command into expr function
        ::llength $list
    }
    proc ll {list} {
        # Wraps `llength` command into expr function
        ::llength $list
    }
    proc lindex {list index args} {
        # Wraps `lindex` command into expr function
        ::lindex $list $index {*}$args
    }
    proc li {list index args} {
        # Wraps `lindex` command into expr function
        ::lindex $list $index {*}$args
    }
    proc lrange {list first last} {
        # Wraps `lrange` command into expr function
        ::lrange $list $first $last
    }
    proc lr {list first last} {
        # Wraps `lrange` command into expr function
        ::lrange $list $first $last
    }
}
proc tcl::mathfunc::maxl {list} {
    # Wraps ::tcl::mathfunc::max command to accept single list instead of expanded one
    return [::tcl::mathfunc::max {*}$list]
}
proc tcl::mathfunc::minl {list} {
    # Wraps ::tcl::mathfunc::min command to accept single list instead of expanded one
    return [::tcl::mathfunc::min {*}$list]
}
proc tcl::mathfunc::logb {value base} {
    # Implements logarithm function with arbitary base
    #  value - value to calculate the logarithm from
    #  base - logarithm base
    # Returns: value of logarithm
    return [= {log($value)/double(log($base))}]
}
