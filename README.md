# Remote-Code-Publisher
A Code Repository is a Program responsible for managing source code resources, e.g., files and documents. A fully developed Repository will support file persistance, managment of versions, and the acquisition and publication of source and document files. A Remote Repository adds the capability to access the Repository's functionality over a communication channel, e.g., interprocess communication, inter-network communication, and communication across the internet.

In this project we will focus on the publication functionality of a Remote Repository. We will develop a remote code publisher, local client, and communication channel that supports client access to the publisher from any internet enabled processor.

The communication channel will use sockets and support an HTTP like message structure. The channel will support:

HTTP style request/response transactions
One-way communication, allowing asynchronous messaging between any two endpoints that are capable of listening for connection requests and connecting to a remote listener.
Transmission of byte streams that are set up with one or more negotiation messages followed by transmission of a stream of bytes of specified stream size2.
The Remote Code Publisher will:

Support publishing web pages that are small wrappers around C++ source code files, just as we did in Project #3.
Accept source code text files, sent from a local client.
Support building dependency relationships between code files saved in specific repository folders, based on the functionality you provided in Project #2 and used in Project #3.
Support HTML file creation for all the files in a specified repository folder1, including linking information that displays dependency relationships, and supports and navigation based on dependency relationships.
Delete stored files, as requested by a local client.
Clients of the Remote Code Publisher will provide a Graphical User Interface (GUI) with means to:

Upload one or more source code text files to the Remote Publisher, specifying a category with which those files are associated1.
Display file categories, based on the directory structure supported by the Repository.
Display all the files in any category.
Display all of the files in any category that have no parents.
Display the web page for any file in that file list by clicking within a GUI control. This implies that the client will download the appropriate webpages, scripts, and style sheets and display, by starting a browser with a file cited on the command line2.
On starting, will download style sheet and JavaScript files from the Repository.
Note that your client does not need to supply the functionality to display web pages. It simply starts a browser to do that. Browsers will accept a file name, which probably includes a relative path to display a web page from the local directory.
