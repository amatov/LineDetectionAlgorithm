wm minsize . 30 30
wm title . "Plime Conic Detection"

frame .cdt
canvas .cdt.canvas -width $DIM_X -height $DIM_Y -background black

label .cdt.temperature -textvariable temp -width 8
pack .cdt.temperature -side bottom

pack .cdt.canvas -fill both
pack .cdt -fill both
