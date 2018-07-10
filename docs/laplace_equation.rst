.. _laplace_equation:

The Laplace Equation
--------------------

Problem description
^^^^^^^^^^^^^^^^^^^

The Laplace equation is one of the most common in physics and
describes a large number of phenomena, including heat transfer.

This project is a simple example of how to implement a trivial
Laplace solver, and in particular how to extend it with reasonable
software engineering practices including an automated build system,
documentation, and some other bells and whistles.

Laplace's equation is a special case of Poisson's equation, and
valid when there are no sources or sinks adding or removing heat
in the system:

.. math:: \Delta u(x,y) = \frac{\partial^2 u}{\partial x^2} + \frac{\partial^2 u}{\partial y^2} = 0

If we discretize this equation on a grid where each cell has side h,
the first index (i) corresponds to x and the second (j) to y, 
and approximate the derivatives from finite differences, we get

.. math:: \left( u_{i,j-1} + u_{i,j+1} + u_{i-1,j} + u_{i+1,j} - 4 u_{i,j} \right) / h^2 = 0,

which we can simplify into (note how h disappears)

.. math:: u_{i,j} = 0.25 \left( u_{i,j-1} + u_{i,j+1} + u_{i-1,j} + u_{i+1,j} \right).

.. image:: /_static/grid_elements.png
    :scale: 50%

Since this has to hold for every element in the grid, we need to iterate over the
grid until the solution converges - and that is the task of this code. There are 
actually significantly more efficient algorithms to accomplish this
(using e.g. over-relaxation), but since the point of this example is to illustrate
software optimization and HPC software engineering practices rather than algorithms
to best solve Laplace's equation we won't implement that since it would complicate
the code.

Orientation of the grid
^^^^^^^^^^^^^^^^^^^^^^^

In physics, the obvious choice is that the first index corresponds to x and the
second to y. In contrast, C++ uses the same row-major memory ordering as C, so in
memory the second index will correspond to continuous memory areas, and after we
have gone through one such 'row', the first index increments. This means the 'rows'
in C++ actually corresponds to the y-axis in the plot above, and in programming
languages we often think of grids/matrices being accessed in top-to-bottom order,
but none of that matters: it's just nomenclature. Just make sure you know how *your*
model corresponds to the memory layout in the code.

Initial and boundary conditions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. image:: /_static/boundary_conditions.png

At the start of the program execution, we will assign the initial value 0 to all
grid cells, but the solution would be pretty boring if we did not have anything
that was non-zero. To achieve this, we set the boundaries of the grid to have the
value 0 for the row/column where the first/second index are zero, and along the
other two boundaries the temperature will increase linearly from 0 to 100, as
illustrated in the figure. Note how we use the computer-centric version of a grid
here, with high index values on the right and bottom.
