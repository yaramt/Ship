# Ship Stowage Plan <br/>
This project is an implementation of a stowage plan for container ships.<br/>
A stowage plan for container ships is the plan by which different types of container vessels are loaded with containers of specific standard sizes. The plans are used to maximize the economy of shipping and safety on board. <br/>  <br/>
![image](https://user-images.githubusercontent.com/98098222/156610179-9d7a10f5-35b5-49e4-b3ce-f861f7c81de1.png) <br/>
https://www.youtube.com/watch?v=kj7ixi2lqF4 <br/>
_________________________________________________________________________________________________________________________________________________________________________________

# Definitions <br/>
- Ship plan<br/>
  A container ship has floors, starting with the lower floor, indexed zero. Each floor has a “plan” which is like a map of the floor.
  An upper floor may add new container spots that were not available in a lower floor, but
  it cannot remove spots that were available in a lower floor.
  We assume all containers are the same size and sit exactly one top of the other.
  The ship’s “plan” is the plan of all its floors. <br/>
- Container <br/>
  A container has the following properties: <br/>
  Weight (kgs) <br/>
  Destination port (5 english letters of seaport code) <br/>
   Unique identifier (according to the ISO 6346 international standard) <br/>
- Ship Route <br/>
  A ship has a finite route, containing any number of destination ports. <br/>
- Cranes operation <br/>
  The dock cranes can pull any container if there is no other container on top of it. <br/>
- Stowage Process  <br/>
  When a ship reaches a port: it unloads all the containers that should be unloaded at this location and loads all the containers that should be loaded to it. <br/>
# Ship’s API <br/>

Ship’s Constructor (1):
Ship(X x, Y y, Height max_height,
std::vector<std::tuple<X, Y, Height>> restrictions,
Grouping<Container> groupingFunctions) noexcept(false);
Ship’s Constructor (2):
Ship(X x, Y y, Height max_height,
std::vector<std::tuple<X, Y, Height>> restrictions) noexcept(false);
This constructor is useful if there are no groupings.
both methods above may throw BadShipOperationException
(see details of this Exception class below).
Ship’s Constructor (3):
Ship(X x, Y y, Height max_height) noexcept;
This constructor is useful if there are no restrictions and no groupings.
So to create a ship, one can do for example:
Ship<int> myShip{X{5}, Y{12}, Height{8}};
Above creates a ship with containers of type int.
loading a container:
void load(X x, Y y, Container c) noexcept(false);
the method may throw BadShipOperationException.
unloading a container:
Container unload(X x, Y y) noexcept(false);
the method may throw BadShipOperationException.
moving a container from one location to another on the ship:
void move(X from_x, Y from_y, X to_x, Y to_y) noexcept(false);
the method may throw BadShipOperationException.
2/5
iterators begin and end:
The ship would only have a const version begin and end iterators for iterating over all
containers on the ship. There is no defined order. Iteration shall not create a copy of the
containers but rather run on the original containers on ship.
getContainersView:
The ship would have the following methods to obtain a “view” of the containers.
The return value of those functions is explained below.
- getContainersViewByPosition(X x, Y y) const;
- getContainersViewByGroup(const string& groupingName, const string& groupName)
const;
functions would not throw an exception, but may return an empty view.
- The view functions would return something of your choice which has iterators begin
and end to allow traversal on the view.
- The view would never be a copy of the containers. If the user calls one of these
functions and holds the result, then loads, unloads or moves a container, then runs
on the view - the run on the view would be on the new data. On the other hand, the
view doesn’t have to support traversing on the view, stopping, then loading,
unloading or moving a container, then continuing the traversal - such operation is not
defined, i.e. load/unload/move operations may invalidate the iterators of a view.
- After a full cycle over the view you cannot traverse over it again, but you can retrieve
the same view again with the proper getContainersView function.
- The order for running on the view:
- getContainersViewByPosition - from the highest container and downwards
- getContainersViewByGroup - order is not important
- *iterator provided by the view would be:
- getContainersViewByPosition - const Container&
- getContainersViewByGroup - std::pair<tuple {X, Y, Height}, const Container&>
BadShipOperationException
has the following ctor: BadShipOperationException(string msg);
- the message is yours, we will not check it, use it as you find suitable
Usage examples
