a work-in-Progress Renderer for OpenGL, written mainly for learning purposes

Things to do to finish the app:
<ul>
<li>Make a cache/ resource manager for models</li>
<li>Rework gui. Make a cegui-layout file. Sliders instead of input fields.</li>
<li>Object Picking:
  <ul>
  <li>Rotate to align to axis</li>
  <li>Helper gui for the user</li>
  <li>maybe: automaticaly put another object on top of the picked one</li>
  </ul></li>
</ul>

Thing that would be good to do:
<ul>
<li><b>rework Scene manager. Its messy! </b></li>
<li>reduce dir shadow acne</li>
<li>get rid of 'washed-out' colors</li>
<li>option to save a scene and load a scene</li>

Random Ideas:
<ul>
<li>Let the material decide what shader is to be used (may be easyier to make materials more flexible this way, ie normal-mapping, specular-mapping, later maybe bump-mapping)</li>
<li><b>Make a resource manager.</b> Class Cache is sort of a small one, but we should also store PhysicShapes now. Also, the
resource manager needs to delete un-used resources. </li>
</ul>

