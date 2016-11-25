a work-in-Progress Renderer for OpenGL, written mainly for learning purposes

To Do:
<ul>
<li><b>Physics:</b> To solve the problem with the floor: Make PhysicObject the parent class of subclasses, ie RigidObject, FloorObject for now. Once a collision is detected, handle it depening on what type of objects collided.</li>
<li><b>rework Scene manager. Its messy! </b></li>
<li>reduce dir shadow acne</li>
<li>Transparent objects, Scene Manager: Use the center of the aabb for distance comparing - <b>done</b></li>
<li>Put everything related to positioning, scaling and rotating object into an own class transform. Let class transform be the mediator between class Scene and the yet-to-be-written class Physics. - <b>done</b></li>
</ul>

Ideas
<ul>
<li>Let the material decide what shader is to be used (may be easyier to make materials more flexible this way, ie normal-mapping, specular-mapping, later maybe bump-mapping)</li>
<li><b>Make a resource manager.</b> Class Cache is sort of a small one, but we should also store PhysicShapes now. Also, the
resource manager needs to delete un-used resources. </li>
</ul>

