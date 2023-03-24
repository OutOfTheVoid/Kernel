# My Kernel
An x86 hobby kernel written in C++. This is a project I embarked on for fun and personal education.

This kernel follows the object oriented principles of C++ where it seems helpful, and otherwise uses features of the language that are deemed convenient. One example of this is the strict use of namespaces for code that does not require C linkage. This allows symbols to be organised and avoid naming conflicts.

This kernel is intended to operate on modern SMP PC's, although it should maintain compatibility with all i686 (or above) IBM-PC hardware, so long as the ACPI is availible. My largest resource in developing this kernel is osdev.org, although i've drawn inspiration and help from many places, including other people's kernels, intel datasheets, and the ACPI specification.

If you have any quesions about the code, please don't hesitate to email me. (See my Github user page).
