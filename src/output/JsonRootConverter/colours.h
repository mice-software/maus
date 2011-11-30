

std::map<const char*,const char*> fg_colours;
std::map<const char*,const char*> bg_colours;
std::map<const char*,const char*> effects;

fg_colours["black"] = "30";
fg_colours["red"] = "31";
fg_colours["green"] = "32";
fg_colours["brown"] = "33";
fg_colours["blue"] = "34";
fg_colours["magenta"] = "35";
fg_colours["cyan"] = "36";
fg_colours["grey"] = "37";

bg_colours["black"] = "40";
bg_colours["red"] = "41";
bg_colours["green"] = "42";
bg_colours["brown"] = "43";
bg_colours["blue"] = "44";
bg_colours["magenta"] = "45";
bg_colours["cyan"] = "46";
bg_colours["white"] = "47";

effects["reset"]="0";
effects["boldON"]="1";
effects["blinkON"]="5";
effects["reverse_fgbgON"]="7";
effects["boldOFF"]="22";
effects["blinkOFF"]="25";
effects["reverse_fgbgOFF"]="27";
