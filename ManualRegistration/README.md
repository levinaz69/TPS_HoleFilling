Manual Registration Toolkit
===========================

Usage
------
#### command line:
ManualRegistration [origin_mesh_file] [template_mesh_file]

#### keyboard interaction: (press key)
* z -- marker selecting mode
* a -- marker adding mode
* i -- marker insert mode
* x -- marker moving mode
* d -- delect selected marker
* v -- toggle simultaneously select(for z, j, k mode)
* j -- select previous marker
* k -- select next marker
* n -- decrease displayed marker size 
* m -- increase display marker size
* g -- save marker list

#### file format:
* mesh file should be in .ply format
* output marker list file in simple point format (.xyz)
* marker list filename: [mesh_file]_marker.xyz

#### interaction tricks:
* use right mouse button for camera rotation
* if pressed control keys, right mouse button will zoom model 
* use simultaneously select mode to check your marker correspondence
