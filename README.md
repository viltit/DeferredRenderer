<em>a work-in-Progress Renderer for OpenGL, written mainly for learning purposes</em>

<h2>Things to do to finish the app:</h2>
<ul>
<li>Make a cache/ resource manager for models   <b>-done</b></li>
<li>Rework gui. Make a cegui-layout file. Sliders instead of input fields.  <b>-done</b></li>
<li>Object Picking:
  <ul>
  <li>Rotate to align to axis     <b>-done</b></li>
  <li>Helper gui for the user</li>
  <li>maybe: automaticaly put another object on top of the picked one</li>
  </ul></li>
  <li>option to save a scene and load a scene</li>
  <li>add an installer, preferably with auto-download of msvs c++ runtime dll's</li>
  <li>detect if the user has an outdated opengl driver and give a message before abort</li>
</ul>

<h2>Thing that would be good to do:</h2>
<ul>
<li><b>rework Scene manager. Its messy! </b></li>
<li>reduce dir shadow acne</li>
<li>get rid of 'washed-out' colors</li>
<li><b>Rework class Cache to a proper resource Manager.</b> <br>
    We need to delete unused resources. Also, we still copy data around, the cache just avoids<br>
    opening and parsing files repeatedly right now</li>
</ul>

<h2>Random Ideas:</h2>
<ul>
<li>Let the material decide what shader is to be used (may be easyier to make materials more flexible this way, ie normal-mapping, specular-mapping, later maybe bump-mapping)</li>
</ul>

