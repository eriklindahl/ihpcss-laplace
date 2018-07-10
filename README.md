# Software Engineering applied to the IHPCSS Laplace code example

### Current Integration status
[![Build Status](https://travis-ci.org/eriklindahl/ihpcss-laplace.svg?branch=master)](https://travis-ci.org/eriklindahl/ihpcss-laplace)
<br>
[![Documentation Status](https://readthedocs.org/projects/ihpcss-laplace/badge/?version=latest)](https://ihpcss-laplace.readthedocs.io/en/latest/?badge=latest)

# A Template Project for Software Engineering
As part of the IHPCSS summer school we show you a number of examples of good practice
in software engineering. However, just knowing about it does not help; you need to 
apply it in your own work, and some of all these tools are not entirely trivial to 
configure.

To help you, I have used the Laplace programming example written by John Urbanic and
introduced some reasonable software engineering standards, including

* A github rebo
* CMake for build configuration
* Continuous integration with Travis to automatically check your build. Travis is
  simpler than Jenkins, and has the advantage of free service for open source projects:
  https://travis-ci.org/eriklindahl/ihpcss-laplace
* High-level documentation with Sphinx to automatically generate HTML and PDF:
  http://http://ihpcss-laplace.readthedocs.io/
* Source-code level documentation with Doxygen
* Unit tests with Google Test (status is reported by Travis CI)
* I've moved the code to use the C++ compiler (in preparation for refactoring)

All these features have also been integrated in CMake so we check if the 
necessary tools are available. For full documentation, consult
the online Sphinx documentation at ReadTheDocs: http://http://ihpcss-laplace.readthedocs.io/

This is of course massive overkill for the simple laplace example. If you want
to have a look at a real-world example you can check out http://www.gromacs.org, but
the advantage with this project is that you can easily clone the repository and
just replace the laplace code with our own project. You are welcome to redistribute
the code in any way you want - no credit necessary.






