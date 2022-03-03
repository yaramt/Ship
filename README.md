# Container Ship Stowage Plan <br/>
This project is an implementation of a stowage plan for a container ship.<br/>
A stowage plan for container ships is the plan by which different types of container vessels are loaded with containers of specific standard sizes. The plans are used to maximize the economy of shipping and safety on board. <br/>  <br/>
![image](https://user-images.githubusercontent.com/98098222/156610179-9d7a10f5-35b5-49e4-b3ce-f861f7c81de1.png) <br/>
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
  When a ship reaches a port, it unloads all the containers that should be unloaded at this location and loads all the containers that should be loaded to it. <br/>
  ________________________________________________________________________________________________________________________________________________________________________________
# Ship’s API <br/>
- Special Global Types for ship: X, Y, Height  <br/>
- Ship’s template parameter: typename Container <br/>
- Ship’s Constructors:  <br/>
  Ship(X x, Y y, Height max_height,std::vector<std::tuple<X, Y, Height>> restrictions,Grouping<Container> groupingFunctions) <br/>
  Ship(X x, Y y, Height max_height,std::vector<std::tuple<X, Y, Height>> restrictions) <br/>
  Ship(X x, Y y, Height max_height) <br/>
- Loading container: void load(X x, Y y, Container c) <br/>
- Unloading container: Container unload(X x, Y y) <br/>
- Moving container from one place to another: void move(X from_x, Y from_y, X to_x, Y to_y) <br/>
- *begin* and *end* iterators iterating over all containers on the ship.  <br/>
- The following functions return a "view" of the ship that can be iterated over using the *begin* and *end* iterators:  <br/>
  getContainersViewByPosition(X x, Y y) <br/>
  getContainersViewByGroup(const string& groupingName, const string& groupName) <br/>
  
  
