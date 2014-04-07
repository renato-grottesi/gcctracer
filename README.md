gcctracer
=========

Using the -finstrument-functions flag in GCC it is possible to inject code in 
the compiled object to execute a special function _cyg_profile_func_enter
before each other function starts and to execute another special function 
_cyg_profile_func_exit before each other function ends.

These two simple functions are the basis to create a fairly simple framework 
where, each time we enter or exit a function, we record the call stack, the 
timestamp, the memory usage and other useful informations.
Such framework can be very effective on debugging applications by introducing 
proactive assertions that can print where the invariant was broken, or by 
allowing smart memory leak detection systems that can print the stack that 
triggered the allocation.
With an external data mining tool, the information about functions interaction 
collected at runtime can also be used to identify bottlenecks, to suggest 
source code optimizations, and to propose source code refactoring.

Check the docs folder for a full list of use cases.
