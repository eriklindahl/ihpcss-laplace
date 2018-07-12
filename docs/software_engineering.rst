.. _software_engineering:

Software Engineering
--------------------

Git & GitHub for source code tracking
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The first practice you must adopt is revision control. As you can see from 
https://github.com/IHPCSS/software-engineering I too went through a number of 
trial-and-error iterations with this repo, and had I not had revision control
I would repeatedly had the issue of trying to fix errors I introduced. With 
revision control, I could simply step back to a known-good version instead.

When you are many developers in a project there are a couple of different
ways through which you can interact. For a large and very active project like
GROMACS, we actually prefer to run our own server where every single commit
is tracked, and then all developers have to continuously rebase their changes
onto the current state of the master branch.

Another approach (which is used e.g. by Linux, and common on GitHub) is that
another developer that wants to contribute to your code sends you a pull
request (PR), and you (the maintainer) then try to pull their code into your
repository to check if it works. Either model is fine.

However, if you are just starting out, we strongly recommend simply creating
an account at https://github.com and put your repos there. GitHub has plenty
of documentation of how to either set up an empty repository or import your
current code, so consult their up-to-date documentation.


Issue Tracking at GitHub
^^^^^^^^^^^^^^^^^^^^^^^^

Even for a small project with a single developer, it is really valuable to have
a list where you can note both bugs, features, and other considerations about
the code. There are many separate advanced tools for this (e.g. redmine), but 
an advantage with GitHub is that each project comes with an integrated issue
tracker - just click the "issues" tab and create a new one.

Each such issue will have a unique number. However, you don't even need to close
them manually: When you add a new git commit that e.g. fixes issue number 14, 
just add a line in the commit message saying

Fixes #14

... and GitHub will automatically mark that issue as closed.

It's a good idea to provide a simple high-level brief documentation for your
GitHub projects. If you just create a file ``README.md`` in the top 
project directory, this will be shown as the default page to users
(see below for some things you can put in this file, or consult the file).

CMake for Build Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you have ever compiled any Unix tool from source, you are likely
familiar with the ``./configure && make && make install`` combo. Build
configuration tools like this check your system (e.g. compiler version,
libraries, what type of build you want, etc) and set up the makefiles
to work for your specific system. They also provide a central location
where you can set things like the version of your project, and then
generate appropriate configuration files for other tools you use.

We prefer a slightly more modern system called CMake (http://cmake.org),
which also has the advantage that it works on Windows and it comes with
modules for CUDA and a bunch of other advanced tools. Most Linux systems
already come with CMake installed. To build the software, create a new
directory (it is a really bad habit to build in the same directory as
your source files, since you cannot tell source from produced files).

Go to your new directory and issue the command

``cmake ../path/to/the/source``

This will produce a bunch of output about what tools are found. If 
any mandatory tool isn't found (say, the compiler) you will get a fatal
error, but otherwise we'll just enable the components we can build.

After the configuration, you can just issue ``make`` to build the code.
We have configured things so that when the build is finished, the binaries
are places in the ``bin`` subdirectory of the build directory you created.
If you want to install the binaries you should set the CMake
variable ``CMAKE_INSTALL_PREFIX`` to a suitable value - the default will
be ``/usr/local``, which means a binary ends up in ``/usr/local/bin``.

You can set such variables on the command line:

``cmake -DCMAKE_INSTALL_PREFIX=/opt/software <path-to-source>``

... or, if you can't remember their name, use ``ccmake <path-to-source>``,
and you will get a slightly more user-friendly interface with a list of
the available options. Rumor has it there are even truly graphical
frontends to cmake, but since you are a hardcore programmer you are 
probably not interested in that.

The preset CMake configuration is extreme overkill for the Laplace solver.
When you only have a single source file, the only thing needed is really
an ``add_executable()`` statement with the name of the binary and corresponding
source file. However, we will gradually make this a more fancy example
with separate libraries and unit tests (just as any project tends to grow),
so it usually pays off to have a clear strategy for how to organize both your
own code and all the other stuff we pull in. We will do this below, but first
we need to describe a couple of the other tools we will use.

By default, CMake will hide most of the build output. If you like to see it,
you can use a command like ``make VERBOSE=1``. For large projects you can also
speed up the build by using multiple compiler threads (this assumes you have
many independent source files to compile) with the argument ``-j N``, where N
is the number of parallel jobs to use. Compilers usually benefit from
SMT/hyperthreading, so you can try number up to twice the number of cores you
have. I like to have 128 threads on my 64-core machine working on 
compiling my large code! 

In particular if you are a beginner it is likely most convenient to use
CMake in the default way described above, but one additional feature that
you might want to try out is that CMake can generate build makefiles for a large
number of build systems, for instance the command-line ninja tool for Linux,
Microsoft Visual Studio, or Apple's Xcode.

Working With CMake
^^^^^^^^^^^^^^^^^^

CMake is a very flexible tool that can be used in a ton of different ways,
but there are a couple of tips that are worth considering. For a typical 
development project, you might have a need both for a debug build to catch
bugs, a release build with full optimization (that can be slower to compile),
and maybe also a release build reasonably high optimization combined with
debug symbols. 

CMake handles this with the variable ``CMAKE_BUILD_TYPE``, whose default 
setting is ``Release``. However, rather than erasing your build and starting
from scratch when you need to change it, you can create a separate directory
for each type of build. Go to each of those directories and configure CMake
with e.g. ``Release``, ``Debug``, and ``RelWithDebInfo``. Similarly, you
can have parallel directories corresponding e.g. to your OpenACC vs. OpenMP
builds. If you now change one file, you just have to type ``make`` in each
such directory, and only the relevant file has to be recompiled and re-linked.

This separation is also used for the compiler flags. CMake uses one variable
(``CMAKE_CXX_FLAGS``) for options that are *not* related to the debug or 
optimization level (where you e.g. add a flag to enable OpenMP), and then
separate variables corresponding to each build type (where optimization flags go):

* ``CMAKE_CXX_FLAGS_RELEASE``
* ``CMAKE_CXX_FLAGS_DEBUG``
* ``CMAKE_CXX_FLAGS_RELWITHDEBINFO``
* etc.

CMake should make reasonable (actually pretty good) default choices, but you can also set variables manually when invoking CMake. For instance, to choose the Intel C++ compiler and enable FMA, we could do

``CXX=icc cmake -DCMAKE_CXX_FLAGS="-mfma" ../software-engineering``

Why do we place CXX first on this line? Well, that isn't really CMake's fault,
but it has long been a Unix standard that the environment variables ``CC`` and
``CXX`` point to the C and C++ compilers, respectively. If you rather prefer
to set the compiler like any other CMake variable, use the CMake flag
``-DCMAKE_CXX_COMPILER=icc`` instead.


Additional CMake Modules
^^^^^^^^^^^^^^^^^^^^^^^^

As you start adding more features to your CMake configuration, such as the
OpenACC detection in this example, you will quickly notice that some of those
features or package-detection modules are only available in very recent
CMake versions. While you *can* require a more recent version of CMake through
the cmake_minimum_required() directive, it is usually a better idea to just
copy the new module (and its dependencies) and put it in our own cmake 
directory - this way we could add OpenACC detection here without bumping our
CMake requirements (the average user will not be amused when you ask them to
update their build tool all the time).


Travis Continuous Integration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

As I showed in the talk at IHPCSS, it is really convenient to have a system
that automatically checks that your build works. For instance, when I developed
this example on my Mac, I made a small mistake in one of the files where I
forgot that Linux requires explicit linking with pthreads, so while it worked
fine on my laptop, the code would fail on Linux. Here too there are very advanced
systems (in GROMACS we use https://jenkins.io), but when you start out it
is likely a pretty big barrier to set up a server where you run everything and
make sure it is up-to-date.

If you are developing an open source project there is a neat completely free
solution (although with slightly fewer bells and whistles than Jenkins) - 
Travis-CI. CI stands for "continuous integration", which effectively means that
every single time you push a new commit to your GitHub repo, Travis-CI will check
out the code and test the build for you.

To enable Travis-CI, go to https://travis-ci.org and follow the instructions. You
will have to log in with your GitHub account and give them permission to sync
your repositories.

By default, Travis is set up to use GNU Autotools configuration instead of CMake.
To fix this, we have created a small file ``.travis.yml`` in the root of the
project that sets the language to C++, and specifies a small script for how to
run the build and tests. You can easily alter this to suit your own code.

Since we are lazy, we prefer not to go to Travis-CI to check if the tests have
passed, so we have simply put a link to a Travis-CI banner in our top README.md
file - this way anyone going to the GitHub repo will instantly see that the
current version passes the build tests.

For the record, there are some limitations with Travis: You cannot easily run
tests that require GPUs or multiple nodes with MPI, and you cannot choose specific
hardware (say, if you want to test a specific SIMD architecture). If you absolutely
need this, it is probably better to look into Jenkins.


General Documentation with Sphinx
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you consider this documentation completely useless and much prefer to decipher
projects by reading the source and Makefiles you can ignore the rest of the
document and head back to the command line. Otherwise, you too might be interested
in how to generate high-level documentation for your project.

We use a tool called Sphinx that reads a very simple format called
reStructuredText (rst). You can have a look at the raw rst files in the ``docs``
subdirectory: as you will see, one advantage is that they are so plain text
files that you can both write them in any plain editor and read them without 
a special program.

However, we'll be slightly more fancy than that. When you run ``cmake``, we check
if the Sphinx tools are installed on your system, and if that is the case you
can later issue the command ``make sphinx-html`` to create neatly formatted
webpages starting at ``docs/html/index.html`` (again, all output will be under
the new directory you created above), or why not build a PDF documentation with
``make sphinx-pdf``? The latter requires that CMake found both Sphinx and 
LaTeX (actually pdflatex). All this high-level documentation is implemented in
the docs subdirectory of your source files, and in the previous chapter you can
also see a few examples of how to include static images.

The file ``docs/conf.py`` contains a few useful settings that you can play around
with. Most of this file was actually auto-generated with sphinx-build, but we
have enabled a couple of extensions. In particular, Sphinx even supports LaTeX
equations in the documentation (again, see previous chapter). The default setup
when displaying such equations in HTML pages is to turn them into (ugly) images,
but Sphinx supports the new MathJAX extensions that enable modern browsers to
show equations natively with TrueType fonts. We have also enabled links back
to the GitHub repo of the code, just to show you how it can be done. If you
are forking this to use for your own code, it is probably a good idea to update
this file so the links point to your repo instead of ours, and mention you as
the author.

But... you don't even need to have Sphinx installed locally! If you go to
https://readthedocs.org, you can do roughly the same as you did for Travis-CI, log
in with your GitHub account, and give ReadTheDocs permission to read your repository.
After this, you can enable ReadTheDocs to automatically build the documentation
for your repository any time you check in changes to GitHub. This way, anyone
can read the documentation at a link like 
https://software-engineering.readthedocs.io/, and you can even provide separate
documentation for multiple different versions of the code in parallel. Both
online and for the local files on your computer, you also have search functionality.

Just as for Travis-CI, the top-level README.md also has a badge to show what the
status of the last documentation build was, so you will be warned if you 
make mistakes, even if you never run Sphinx locally.

Code Documentation with Doxygen
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

While Sphinx provides a way to write manual high-level documentation, the goal
of Doxygen is to automatically parse your code and generate documentation about
every single public class, interface, function, and generate webpages where you
can just click an argument to get more information about the type.

If you have the right software installed, Doxygen can also generate class
diagrams of your C++ classes so you can see how they depend on each other, and
make sure there are no circular dependencies. This requires the ``dot`` tool
from the GraphViz package (http://graphviz.org). If you try to compile it 
yourself, note that you need PNG support, which is unfortunately a bit difficult
to enable on some systems - it might be easiest to download a binary version
instead.

If you were using Doxygen in stand-alone mode, you would have to
edit the configuration file every time the project version changed and/or to
alter settings like whether the ``dot`` tool is available, but we handle all this
with CMake, where we have fully integrated Doxygen support.
There is an input template file (Doxyfile.cmakein) with a couple
of variables that will be replaced by their CMake values, and then we write out
the Doxyfile configuration file that is actually used by doxygen.
To generate the source code documentation, simply issue ``make doxygen``.
The resulting output will be available in the (usual) output directory, under 
``docs/doxygen/html/index.html``, and there are also LaTeX files if you
want to integrate it with your manual or something.


Unit tests with GoogleTest
^^^^^^^^^^^^^^^^^^^^^^^^^^

Knowing that every version of your code compiles is good, but knowing that
it also produces correct results is far better. There are a couple of ways
to achieve this. One of the most common one is to have a collection of
examples where you know the answer and always check that you still get the
same answer (called *regression tests*). While this might sound good, the
problem is that there might have been a bug since the first version of your
code, and in that case you are merely testing that you still have the same
bug. Another problem is as your program grows, it can become very difficult
to find the bug. If you only test things every few months and have a million
lines of code in a very active project (and large commits....) it could take
you weeks to trace down the location of the problem - and then you haven't
even begun fixing it.

A better approach to modern software engineering is *unit tests*. The key
idea with this is that you should design your code into small independent
modules with a clear interface (only a handful of functions in each module),
and no other code should be able to touch data inside the module. Then,
before you even start coding, you should define exactly what you accept
as correct answers by this module and how to test it.

Note that you should ideally define your unit tests *before* you even start
implementing the code - the module will be done when it passes the unit
tests. Your first reaction to this is likely going to be that it takes too
much time to write these tests, but after having used them for a while you
will hopefully see that they change everything. If your modules are small,
without circular dependencies, and have exhaustive unit tests, the 
continuous integration testing will show you exactly in what 20-30 lines
of code a bug is - before you have even opened the source code! We have
caught hundreds of bugs not only in our own code but also in compilers,
operating systems and even hardware binary programming interfaces this way.
It is not a coincidence this is the way software is developed in industry.

In modern software development we sometimes talk about *code coverage*, which
is simply the fraction of your code covered by unit tests, which is completely
different from regression tests. We won't lie and claim it's easy to achieve
100%, but it is much easier to achieve a high fraction by being serious
with the unit tests from the start, before you have a gigantic codebase.

In theory you could just write your own small test programs, but that quickly
becomes very tedious, not to mention you also want to report to the developer
how it failed (i.e., what value we expected compared to what it was). There
are a number of different *testing frameworks* that can help you with this.
We like to use GoogleTest, mostly because it is small and *very* portable.

You can find information about how to write tests at
https://github.com/google/googletest/blob/master/googletest/docs/primer.md,
or just look at our test files (see source code organization, below). We have
fully integrated GoogleTest in CMake in this project; you don't even need
to install it, since we have copied the handful of files we need into the
project.

To run the tests after CMake configuration, issue the command ``make check``.
This will first build all the tests, run them, and report the results.
If you go back and check the Travis-CI script we wrote, you can see that we
include this step there, so Travis will actually run all the unit tests for
you every time we test the project.


Source Code Directory Organization
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

There are more tools you can use, but already with these you can imagine
things can get a bit complex in the repository. There is no unique way that
source code must be organized, but here's a suggestion we like:

* First, we like having a clean top-level directory. The ``README.md`` file must
  go here, as must ``CMakeLists.txt``, and the ``docs`` directory. We also
  have a separate ``cmake`` directory where we place all the other files/modules
  CMake might need (for instance, the module to detect Sphinx).
* Second, we create a ``src`` directory for all the source. This is not limited
  to our own source, but we also need a place to store things like GoogleTest
  files. I like to handle this by having an ``external`` subdirectory for
  everything that is *not* my project, and then a subdirectory with the same
  name as my project (``laplace`` here) for our own files.
* For a simple project, you could place all your source files directly in the
  latter of these subdirectories, but let's plan ahead a bit. At some point you
  might want to move common code from several files to a library, and also 
  organize different modules into separate directories. To prepare for this,
  we add yet another layer called ``programs`` where we have the source for
  the actual executable. Before you go crazy about all the directories, remember
  that CMake will handle most things automatically for you, and the resulting
  binary will be placed under ``bin`` in the top-level output directory!
* Remember the unit tests? We like to keep each unit test *really* close to the
  module it is testing, so in each lowest-level directory (like ``programs``) we
  create a ``tests`` subdirectory. You can have a look at how CMakeLists.txt
  includes subdirectories, how the test directories are only included 
  if we build the unit tests, and how we use a small macro to register each such
  unit test with CMake, so they are all executed by ``make check`` (there is some
  magic code in CMakeLists.txt in the src directory that accomplishes this, which
  in turn uses the TestMacros.cmake file from the cmake directory).





