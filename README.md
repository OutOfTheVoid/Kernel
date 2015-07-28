# My Kernel
An x86 hobby kernel written in C++. This is a project i've embarked on for fun and personal education. Throughout my years, i've attempted to write a kernel a handful of times, with varying degrees of success, but this one feels like the one that will make it.

This kernel follows the object oriented principles of C++ where it seems helpful, and otherwise uses features of the language that are deemed convenient. One example of this is the strict use of namespaces for code that does not require C linkage. This allows symbols to be organised in a descriptive and less verbose way which avoids naming conflicts.

This kernel is intended to operate on modern SMP PC's, although it should maintain compatibility with all i686 (or above) IBM-PC hardware, so long as the ACPI is availible. My largest resource in developing this kernel is osdev.org, although i've drawn inspiration and help from many places, including other people's kernels, intel datasheets, and the ACPI specification.

If you have any quesions about the code, please don't hesitate to email me, although I do not want to help you with a problem that you should be able to figure out with a few google searches or a bit of thorough debugging. My email is posted on my Github user page.

Currently the code is unlicensed and completely open source, but I plan to put a real open-source license on it when i find the time, so for now, please use it how you want to, within reason!
