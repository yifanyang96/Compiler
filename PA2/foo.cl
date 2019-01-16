(* This is comment *)
-- This is a valid -- comment
class CellularAutomaton inherits IO {
    population_map : String;

    init(map : String) : SELF_TYPE {
        {
            population_map <- map;
            1+2;
            3*4;
            5-6;
            1.2;
            1>2;
            self;
	    tRuE;
        }
    };
    test() {
      String a <- "  ";
      String b <- "\n";
      String c <- "
      ";
      String d <- "";
      String e <- "123"
      String f <- "\3"
    }
};
(* This is bad comment
