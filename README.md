gcctracer
=========

gcctracer will create a trace of all functions called in your applications.
Each trace line will contain useful information for the function invocation
such as:
 * function name
 * place of invocation
 * allocated memory
 * thread number
 * ...

The infomation is collected by gcctrace providing an implementaion for gcc's
instrument function functionality.

There will be some visualizers to visually display the data collected and do
data mining.

