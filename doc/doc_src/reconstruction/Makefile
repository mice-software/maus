all:
	dot -Tpdf class_hierarchy_main.dot -oclass_hierarchy_main.pdf
	dot -Tpng class_hierarchy_main.dot -oclass_hierarchy_main.png
	dot -Tpdf class_hierarchy_interfaces.dot -oclass_hierarchy_interfaces.pdf
	dot -Tpng class_hierarchy_interfaces.dot -oclass_hierarchy_interfaces.png
	pdflatex global_track_reconstruction_specification.tex
	cp global_track_reconstruction_specification.pdf ../../

clean:
	rm -f global_track_reconstruction_specification.aux global_track_reconstruction_specification.dvi global_track_reconstruction_specification.log global_track_reconstruction_specification.toc global_track_reconstruction_specification.pdf class_hierarchy_main.eps class_hierarchy_interfaces.jpg  class_hierarchy_main.jpg class_hierarchy_interfaces.pdf  class_hierarchy_main.pdf class_hierarchy_interfaces.png  class_hierarchy_main.png class_hierarchy_main.svg


