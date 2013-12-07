/* There is a total of about 20 tiles; that can be rotated producing 80
 * variants.
 */
#include <stdlib.h>

//#define SLIDE(THICKA,THICKB) 0.15
//#define SLIDE(THICKA,THICKB) 0.72
//
//#define MORPH      0.3
//
float MORPH = 0.8;
//#define MORPH      0.8
#define SLIDE(VALA, VALB)  (MORPH * VALB + (1.0-MORPH)* VALA)

#define THICKA  0.72
#define THICKB  0.05

/* helper generator script for generating correctly winded symmetric shapes
 * for the 0xA000 palette.
 */

#include <stdio.h>

typedef struct Vertex {
  char type;
  float x;
  float y;
} Vertex;

Vertex shape[64];
int n_vertex = 0;

void clear (void)
{
  n_vertex = 0;
}

void rot90 (void) /* rotates shape in unit square 90 degrees */
{
  int i;
  for (i = 0; i < n_vertex; i++)
    {
      float u, v;
      u = shape[i].x;
      v = shape[i].y;

      shape[i].x = 1.0-v;
      shape[i].y = u;
    }
}

void vertex (char type, float x, float y)
{
  shape[n_vertex].type = type;
  shape[n_vertex].x = x;
  shape[n_vertex].y = y;
  n_vertex++;
}

void name (const char *name)
{
  int i;
  fprintf (stdout, "{ %s\n", name);
  for (i = 0; i < n_vertex; i++)
    {
      if (shape[i].type == 'Z')
        fprintf (stdout, "Z\n");
      else
      fprintf (stdout, "%c %f %f\n", shape[i].type, shape[i].x, shape[i].y);
    }
  fprintf (stdout, "\n");
}

#define NEW clear();

void line_to (float x, float y)
{
  vertex  ('L', x, y);
}

void curve_to (float x0, float y0,
               float x1, float y1,
               float x2, float y2)
{
  vertex ('c', x0, y0);
  vertex ('c', x1, y1);
  vertex ('C', x2, y2);
}

void new_path ()
{
  vertex ('Z', 0, 0);
}

int main (int argc, char **argv)
{
  if (argv[1])
    MORPH = atof (argv[1]);

  NEW
  line_to (SLIDE(0,0), SLIDE(0.80, 0.45));
  line_to (1.0,   0);
  line_to ((1.0-SLIDE(THICKA,THICKB)),  0);
  line_to (0.0,   SLIDE(0.20, 0.55));
  name ("specials2");
  rot90 ();
  rot90 ();
  name ("specials1");

  NEW
  line_to (0, 1.0);
  line_to (1.0, 0);
  line_to ((1.0-SLIDE(THICKA,THICKB)), 0);
  line_to (0.0, (1.0-SLIDE(THICKA,THICKB)));
  name ("lne2");
  rot90 ();
  name ("lnw2");
  name ("lnw3");

  NEW
  line_to (0, SLIDE(THICKA,THICKB));
  line_to (SLIDE(THICKA,THICKB), 0);
  line_to (0, 0);
  line_to (0.0, 0);
  rot90 ();
  rot90 ();
  name ("lsw2");
  rot90 ();
  name ("lse2");

  NEW
  line_to (0, SLIDE(THICKA,THICKB));
  line_to (SLIDE(THICKA,THICKB), 0);
  line_to (0, 0);
  name ("lne");
  rot90 ();
  name ("lnw");

  NEW
  line_to (0, 1);
  line_to (1, 0);
  line_to ((1.0-SLIDE(THICKA,THICKB)), 0);
  line_to (0.0, (1.0-SLIDE(THICKA,THICKB)));
  rot90 ();
  rot90 ();
  name ("lsw");
  rot90 ();
  name ("lse");
  new_path ();
  line_to (0, SLIDE(THICKA,THICKB));
  line_to (1, SLIDE(THICKA,THICKB));
  line_to (1, 0);
  line_to (0, 0);

  name ("lse3");
  rot90();
  rot90();
  name ("lnw3");


  NEW
  line_to ((1.0-SLIDE(THICKA,THICKB)), 0);
  curve_to (SLIDE(0.22,0.8), SLIDE(0.13, 0.97),
            SLIDE(0.43,0.10), SLIDE(0.13, 0.97),
            0,    SLIDE(0.27, 0.97));
  line_to (0, 1);
  curve_to (0.5523, 1,
            1, 0.5523,
            1, 0);
  name ("cne");
  rot90 ();
  name ("cnw");
  rot90 ();
  name ("csw");
  rot90 ();
  name ("cse");

  NEW
  line_to (0, 1);
  curve_to (0.5523, 1,
            1,      0.5523,
            1,      0);
  line_to (1, 1);
  name ("Cne");
  rot90 ();
  name ("Cnw");
  rot90 ();
  name ("Csw");
  rot90 ();
  name ("Cse");

  NEW
  line_to (1, SLIDE(0, 0.8));
  curve_to (0.7,  SLIDE(0.28, 0.8),
            0.28, SLIDE(0.28, 0.9),
            0, SLIDE(0, 0.8));
  line_to (0, 0.9);
  curve_to (0.21, 1.0, 
            0.65, 1.0,
            1, 0.9);
  name ("c8");
  rot90 ();
  name ("c4");
  rot90 ();
  name ("c2");
  rot90 ();
  name ("c6");


  NEW
  line_to (0, 1-SLIDE(THICKA,THICKB));
  line_to (0, 1);
  line_to (0.5, 0.5);
  line_to (1, 1);
  line_to (1, 1-SLIDE(THICKA,THICKB));
  line_to (SLIDE(0.8,0.3), 0);
  line_to (SLIDE(0.2,0.7), 0);
  name ("vn");
  rot90 ();
  name ("vw");
  rot90 ();
  name ("vs");
  rot90 ();
  name ("ve");

  NEW

  line_to (0, SLIDE(0.76, 0.05));
  line_to (0.5, SLIDE(0.36, 0.19));
  line_to (1, SLIDE(0.76, 0.05));
  line_to (1, 0);
  line_to (0, 0);
  name ("vn2");
  rot90 ();
  name ("vw2");
  rot90 ();
  name ("vs2");
  rot90 ();
  name ("ve2");


  NEW
  line_to (0, 1);
  line_to (1, 1);
  line_to (1, (1.0-SLIDE(THICKA,THICKB)));
  line_to (0, (1.0-SLIDE(THICKA,THICKB)));
  name ("solidNorth");
  rot90 ();
  name ("solidWest");
  rot90 ();
  name ("solidSouth");
  rot90 ();
  name ("solidEast");

  NEW
  line_to (1, 1);
  line_to (0.5, 0.5);
  line_to (0, 1);
  name ("Vn");
  rot90 ();
  name ("Ve");
  rot90 ();
  name ("Vs");
  rot90 ();
  name ("Vw");


  NEW
  line_to (1, 0.1);
  curve_to (0.56, 0.30,
            0.27, 0.62,
            0.10, 1);
  line_to (SLIDE(1,0.21), 1);
  curve_to (SLIDE(0.8,0.45), SLIDE(0.7, 0.75),
           SLIDE(0.9,0.7), SLIDE(0.5,0.35),
           1, SLIDE(0.5,0.2));
  new_path ();
  line_to (0, SLIDE(THICKA,THICKB));
  line_to (1, SLIDE(THICKA,THICKB));
  line_to (1, 0);
  line_to (0, 0);
  name ("cj1");
  rot90();
  name ("cj3");
  rot90();
  name ("cj9");
  rot90();
  name ("cj7");

  NEW
  line_to (1, 0.1);
  curve_to (0.56, 0.30,
            0.27, 0.62,
            0.10, 1);
  line_to (SLIDE(1,0.21), 1);
  curve_to (SLIDE(0.8,0.45), SLIDE(1, 0.75),
           SLIDE(0.9,0.7), SLIDE(1,0.35),
           1, SLIDE(1,0.23));
  name ("c1");
  rot90 ();
  name ("c3");
  rot90 ();
  name ("c9");
  rot90 ();
  name ("c7");

  NEW
  line_to (0.1, 0);
  curve_to (0.0, 0.79, 0.0, 0.65, 0.0, 1);
  line_to (SLIDE(THICKA,THICKB), 1);
  curve_to (SLIDE(THICKA,THICKB), SLIDE(0.3,0.1),
            SLIDE(0.9,0.3), 0.1,
            SLIDE(1, 0.05), 0);
  name ("c4b");
  rot90 ();
  name ("c2b");
  rot90 ();
  name ("c6a");
  rot90 ();
  name ("c8a");

  NEW
  line_to (SLIDE(1, 0.2), 1);
  curve_to (SLIDE(0.9,0.1), SLIDE(0.9, 0.3),
            SLIDE(THICKA,THICKB),      SLIDE(0.7, 0.3),
            SLIDE(THICKA,THICKB),      0);
  line_to (0, 0);
  curve_to (0.0, 0.35,
            0.0, 0.11,
            0.10, 1);
  name ("c4a");
  rot90 ();
  name ("c2a");
  rot90 ();
  name ("c6b");
  rot90 ();
  name ("c8b");

  NEW
  line_to (0, SLIDE(THICKA,THICKB));
  line_to (1, SLIDE(THICKA,THICKB));
  line_to (1, 0);
  line_to (0, 0);
  new_path ();
  line_to (0, 0);
  line_to (0, 1);
  line_to (SLIDE(THICKA,THICKB), 1);
  line_to (SLIDE(THICKA,THICKB), 0);
  name ("solidSouthWest");
  rot90 ();
  name ("solidSouthEast");
  rot90 ();
  name ("solidNorthEast");
  rot90 ();
  name ("solidNorthWest");

  NEW
  line_to (0, 1);
  line_to (1, 1);
  line_to (1, (1.0-SLIDE(THICKA,THICKB)));
  line_to (0, (1.0-SLIDE(THICKA,THICKB)));
  new_path();
  line_to (0, (1.0-SLIDE(THICKA,THICKB)));
  line_to (0, 1);
  line_to (SLIDE(THICKA,THICKB), 1);
  line_to (SLIDE(THICKA,THICKB), (1.0-SLIDE(THICKA,THICKB)));
  name ("solidSouthWest5");

  NEW
  line_to (0, SLIDE(0.1, 0.1));
  line_to (0, SLIDE(1.0, 0.2));
  line_to (1, SLIDE(0.9, 0.9));
  line_to (1, SLIDE(0, 0.8));
  name ("specialS");

  NEW
  line_to (0, SLIDE(0, 0));
  line_to (0, SLIDE(THICKA,THICKB));
  line_to (1, SLIDE(1, 1));
  line_to (1, 1.0-SLIDE(THICKA,THICKB));
  name ("smallcapss");

  NEW
  line_to (0, (1.0-SLIDE(THICKA,THICKB)));
  line_to (0, 1.0);
  line_to (1, 1.0);
  line_to (1, 0);
  name ("specialB");

#define MIDS ((1.0-SLIDE(THICKA,THICKB))/2)

  NEW
  line_to (MIDS, 0);
  line_to (MIDS, 1.05);
  line_to (1.0-MIDS, 1.05);
  line_to (1.0-MIDS, 0);
  name ("solidMiddle");

  NEW
  line_to (MIDS, 0);
  line_to (MIDS, 1.0);
  line_to (1.0-MIDS, 1.0);
  line_to (1.0-MIDS, 0);
  new_path ();
  line_to (0, 1);
  line_to (1, 1);
  line_to (1, (1.0-SLIDE(THICKA,THICKB)));
  line_to (0, (1.0-SLIDE(THICKA,THICKB)));
  name ("solidMiddleNorth");
  rot90 ();
  rot90 ();
  name ("solidMiddleSouth");

  NEW
  line_to (1, SLIDE(0, 0.77));
  line_to (0.8, 0.2);
  line_to (SLIDE(0.7,0.3), SLIDE(0,0.7));
  line_to (0.7, 0.4);
  curve_to (0.27, 0.38,
            0.56, 0.70,
            1, 0.9);
  new_path ();
  line_to (0, 0.9);
  curve_to (0.44, 0.70,
            0.73, 0.38,
            0.90, 0);
  line_to (SLIDE(0.3, 0.66), 0);
  line_to (0, SLIDE(0.0, 0.75));
  name ("v13");
  rot90 ();
  name ("v17");
  rot90 ();
  name ("v79");
  rot90 ();
  name ("v39");

  return 0;
}
