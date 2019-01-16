(* This is comment *)
class CellularAutomaton inherits IO {
    population_map : String;

    init(map : String) : SELF_TYPE {
        {
            population_map <- map;
            1+2;
            while 3<4 loop 5 pool;
            if 2<3 then 4 else 5 fi;
            self;
        }
    };
    test() : String {
      {let a : String, b : Int, c : String in a <- c;}
    };
};
