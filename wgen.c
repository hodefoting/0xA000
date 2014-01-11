/* There is a total of about 20 tiles; that can be rotated producing 80
 * variants.
 */
#include <stdlib.h>

//#define SLIDE(0.72,0.05) 0.15
//#define SLIDE(0.72,0.05) 0.72
//
//#define MORPH      0.3
//
float MORPH = 0.8;
//#define MORPH      0.8
#define SLIDE(VALA, VALB)  (MORPH * VALB + (1.0-MORPH)* VALA)

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

void vertex_scaled (int type, float x, float y, float scale)
{
  x -= 0.5;
  y -= 0.5;
  scale *= 1.5;
  x *= scale;
  y *= scale;

  x += 0.5;
  y += 0.5;
  x += SLIDE (0.18, 0.44);

  fprintf (stdout, "%c %f %f\n", type, x, y);
}

void vertex_scaled2 (int type, float x, float y, float scale)
{
  x -= 0.5;
  y -= 0.5;
  scale *= 1.5;
  x *= scale;
  y *= scale;

  x += 0.5;
  y += 0.5;

  fprintf (stdout, "%c %f %f\n", type, x, y);
}

void vertex_scaled3 (int type, float x, float y, float scale)
{
  x -= 0.5;
  y -= 0.5;
  scale *= 1.5;
  x *= scale;
  y *= scale;

  x += 0.5;
  y += 0.5;

  y -= 0.5;

  fprintf (stdout, "%c %f %f\n", type, x, y);
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
  line_to (0.0, SLIDE(0.78, 0.525));
  line_to (1.0, 0);
  line_to (SLIDE(0.28, 0.95), 0);
  line_to (SLIDE(0,0), SLIDE(0.22, 0.475));
  name ("specials2");
  rot90 ();
  rot90 ();
  name ("specials1");

  NEW
  line_to (0, SLIDE(0.72,0.35));
  line_to (SLIDE(0.88,0.53), 0);
  line_to (SLIDE(0.12,0.47),  0);
  line_to (0, SLIDE(0.1,0.30));
  name ("specialSa");
  rot90 ();
  rot90 ();
  name ("specialSb");

  NEW
  line_to (0, SLIDE(0.9,0.7));
  line_to (1, 0);
  line_to (SLIDE(0.28,0.95), 0);
  line_to (0.0, SLIDE(0.28,0.65));
  name ("foo");
  rot90 ();
  rot90 ();
  name ("bar");

  NEW
  line_to (0, 1.0);
  line_to (1.0, 0);
  line_to ((1.0-SLIDE(0.72,0.05)), 0);
  line_to (0.0, (1.0-SLIDE(0.72,0.05)));
  name ("lne2");
  rot90 ();
  name ("lnw2");

  NEW
  line_to (0, SLIDE(0.72,0.05));
  line_to (SLIDE(0.72,0.05), 0);
  line_to (0, 0);
  line_to (0.0, 0);
  rot90 ();
  rot90 ();
  name ("lsw2");
  rot90 ();
  name ("lse2");

  NEW
  line_to (0, SLIDE(0.72,0.05));
  line_to (SLIDE(0.72,0.05), 0);
  line_to (0, 0);
  name ("lne");
  rot90 ();
  name ("lnw");

  NEW
  line_to (0, 1);
  line_to (1, 0);
  line_to ((1.0-SLIDE(0.72,0.05)), 0);
  line_to (0.0, (1.0-SLIDE(0.72,0.05)));
  rot90 ();
  rot90 ();
  name ("lsw");
  rot90 ();
  name ("lse");
  new_path ();
  line_to (0, SLIDE(0.72,0.05));
  line_to (1, SLIDE(0.72,0.05));
  line_to (1, 0);
  line_to (0, 0);
  name ("lse3");
  rot90();
  name ("lne3");
  rot90();
  name ("lnw3");
  rot90();
  name ("lsw3");

  NEW
  line_to ((1.0-SLIDE(0.72,0.05)),       0);
  curve_to (SLIDE(0.27,0.95), SLIDE(0.13, 0.51),
            SLIDE(0.17,0.51), SLIDE(0.27, 0.95),
            0,                SLIDE(0.27, 0.95));
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
  line_to (1,     SLIDE(0, 0.83));
  curve_to (0.7,  SLIDE(0.28, 0.94),
            0.3,  SLIDE(0.28, 0.94),
            0,    SLIDE(0, 0.83));
  line_to (0, 0.9);
  curve_to (0.3, 1.0, 
            0.7, 1.0,
            1, 0.9);
  name ("c8");
  rot90 ();
  name ("c4");
  rot90 ();
  name ("c2");
  rot90 ();
  name ("c6");


  NEW
  line_to (0, 1-SLIDE(0.72,0.05));
  line_to (0, 1);
  line_to (0.5, 0.5);
  line_to (1, 1);
  line_to (1, SLIDE(0.28, 0.95));
  line_to (SLIDE(0.73,0.5), SLIDE(0, 0.45));
  line_to (SLIDE(0.28,0.5), SLIDE(0., 0.45));
  name ("vn");
  rot90 ();
  name ("vw");
  rot90 ();
  name ("vs");
  rot90 ();
  name ("ve");
  NEW
  line_to (0, SLIDE(0.72, 0.05));
  line_to (SLIDE(0.72,0.05), 0);
  line_to (0, 0);
  new_path ();

  line_to (1, 0);
  line_to (1.0-SLIDE(0.72,0.05), 0);
  line_to (1, SLIDE(0.72, 0.05));
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
  line_to (1, (1.0-SLIDE(0.72,0.05)));
  line_to (0, (1.0-SLIDE(0.72,0.05)));
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
  line_to (SLIDE(0.5, 0.05), 1);
  line_to (0.5, SLIDE(1.4, 0.55)); /* ugly hack? */
  line_to (SLIDE(0.5, 0.95), 1);
  name ("Vn");
  rot90 ();
  name ("Ve");
  rot90 ();
  name ("Vs");
  rot90 ();
  name ("Vw");

  NEW
  line_to (1, 0.0);
  curve_to (0.56, 0.30,
            0.27, 0.62,
            0.10, 1);
  line_to (SLIDE(1,0.15), 1);
  curve_to (SLIDE(1,0.42), SLIDE(1, 0.5),
           SLIDE(1,0.6), SLIDE(0.72,0.37),
           1, SLIDE(0.72,0.05));
  new_path ();
  line_to (0, SLIDE(0.72,0.05));
  line_to (1, SLIDE(0.72,0.05));
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
  line_to (0, SLIDE(0.72,0.05));
  curve_to(SLIDE(0,0.4), SLIDE(0.72,0.37),
           SLIDE(0,0.58), SLIDE(1, 0.5),
           SLIDE(0,0.85), 1);
  line_to  (0.90, 1);
  curve_to (0.73, 0.62,
            0.44, 0.30,
            0, 0.0);
  new_path ();
  line_to (0, SLIDE(0.72,0.05));
  line_to (1, SLIDE(0.72,0.05));
  line_to (1, 0);
  line_to (0, 0);
  name ("cj1b");
  rot90();
  name ("cj3b");
  rot90();
  name ("cj9b");
  rot90();
  name ("cj7b");

  NEW
  line_to (1, 0.1);
  curve_to (0.56, 0.30,
            0.27, 0.62,
            0.10, 1);
  line_to (SLIDE(1,0.17),  1);
  curve_to (SLIDE(1,0.28), SLIDE(1, 0.70),
            SLIDE(1,0.70), SLIDE(1, 0.28),
            1,             SLIDE(1, 0.17));
  name ("c1");
  rot90 ();
  name ("c3");
  rot90 ();
  name ("c9");
  rot90 ();
  name ("c7");

  NEW
  line_to (0.1, 0);
  curve_to (0.0, 0.49, 0.0, 0.65, 0.0, 1);
  line_to (SLIDE(0.72,0.05), 1);
  curve_to (SLIDE(0.72,0.05), 0.4,
            SLIDE(0.79, 0.16), 0.3,
            SLIDE(1, 0.16), 0);
  name ("c4b");
  rot90 ();
  name ("c2b");
  rot90 ();
  name ("c6a");
  rot90 ();
  name ("c8a");

  NEW
  line_to  (SLIDE(1, 0.16), 1);
  curve_to (SLIDE(0.79, 0.16), 0.7,
            SLIDE(0.72,0.05), 0.6,
            SLIDE(0.72,0.05), 0);
  line_to  (0.0, 0);
  curve_to (0.0, 0.35,
            0.0, 0.51,
            0.1, 1);
  name ("c4a");
  rot90 ();
  name ("c2a");
  rot90 ();
  name ("c6b");
  rot90 ();
  name ("c8b");

  NEW
  line_to (0, SLIDE(0.72,0.05));
  line_to (SLIDE(0.72,0.05), SLIDE(0.72,0.05));
  line_to (SLIDE(0.72,0.05), 0);
  line_to (0, 0);
  name ("box1");
  rot90 ();
  name ("box3");
  rot90 ();
  name ("box9");
  rot90 ();
  name ("box7");

  NEW
  line_to (0, SLIDE(0.72,0.05));
  line_to (1, SLIDE(0.72,0.05));
  line_to (1, 0);
  line_to (0, 0);
  new_path ();
  line_to (0, 0);
  line_to (0, 1);
  line_to (SLIDE(0.72,0.05), 1);
  line_to (SLIDE(0.72,0.05), 0);
  name ("solidSouthWest");
  rot90 ();
  name ("solidSouthEast");
  rot90 ();
  name ("solidNorthEast");
  rot90 ();
  name ("solidNorthWest");

  NEW
  line_to (0, SLIDE(0, 0));
  line_to (0, SLIDE(0.72,0.05));
  line_to (1, SLIDE(1, 1));
  line_to (1, 1.0-SLIDE(0.72,0.05));
  name ("smallcapss");

#define MIDS ((1.0-SLIDE(0.72,0.05))/2)

  NEW
  line_to (MIDS, 0);
  line_to (MIDS, 1.05);
  line_to (1.0-MIDS, 1.05);
  line_to (1.0-MIDS, 0);
  name ("solidMiddle");
  rot90 ();
  name ("solidMiddleHorizontal");

  NEW
  line_to (MIDS, 0);
  line_to (MIDS, 1.0);
  line_to (1.0-MIDS, 1.0);
  line_to (1.0-MIDS, 0);
  new_path ();
  line_to (0, 1);
  line_to (1, 1);
  line_to (1, (1.0-SLIDE(0.72,0.05)));
  line_to (0, (1.0-SLIDE(0.72,0.05)));
  name ("solidMiddleNorth");
  rot90 ();
  name ("solidMiddleWest");
  rot90 ();
  name ("solidMiddleSouth");

  NEW
  line_to (MIDS, 0);
  line_to (MIDS, 1.0);
  line_to (1.0-MIDS, 1.0);
  line_to (1.0-MIDS, 0);
  new_path ();
  line_to (0, 1.0-MIDS);
  line_to (1, 1.0-MIDS);
  line_to (1, MIDS);
  line_to (0, MIDS);
  name ("middleCross");

  NEW
  line_to (1, SLIDE(0.01, 0.83));
  curve_to (SLIDE(1, 0.65), SLIDE(0, 0.64),
            SLIDE(0.86,0.525), SLIDE( 0, 0.45),
            SLIDE(0.86,0.525), 0);
  line_to (SLIDE(0.14,0.475), 0.0);
  curve_to (SLIDE(0.14,0.475), 0.38,
            0.56, 0.70,
            1, 0.9);
  new_path ();
  line_to (0, 0.9);
  curve_to (SLIDE(0.44, 0.44), 0.70,
            SLIDE(0.86,0.525), SLIDE(0.2, 0.38),
            SLIDE(0.86,0.525), 0);
  line_to (SLIDE(0.38, 0.475), 0);
  curve_to (SLIDE(0.38, 0.475), SLIDE(0, 0.45),
            SLIDE(0, 0.35), SLIDE(0, 0.64),
            0, SLIDE(0.0, SLIDE(0.75, 0.83)));
  name ("v13");
  rot90 ();
  name ("v17");
  rot90 ();
  name ("v79");
  rot90 ();
  name ("v39");

  NEW
  line_to (0.540000, 0.600000);
  curve_to (0.760000,0.500000,
            0.790000,0.400000,
            0.900000,0.000000);
  line_to (SLIDE(0.0,0.830000), 0.000000);
  curve_to (SLIDE(0,0.720000),0.400000,
            SLIDE(0,0.690000),SLIDE(0.1,0.450000),
            SLIDE(0,0.540000),SLIDE(0.1, 0.530000));
  name ("rSpecial");


  NEW
  line_to (SLIDE(0.00, 0.83), 1.0);
  line_to (0.9, 1.0);
  line_to (0.0, 0.1);
  line_to (0.0, SLIDE(1.0, 0.17));
  name ("eSpecial2");

  NEW
  line_to (0.9, 0.0);
  line_to (SLIDE(0, 0.83), 0.0);
  line_to (1.0, SLIDE(1.0, 0.17));
  line_to (1.0, 0.1);
  name ("eSpecial1");


  NEW
  line_to (1.0, SLIDE(1.0, 0.17));
  line_to (1.0, 0.1);
  line_to (0.1, 1.0);
  line_to (SLIDE(1.00, 0.17), 1.0);
  name ("eSpecial2b");

  NEW
  line_to (0.0, 0.1);
  line_to (0.0, SLIDE(1.0, 0.17));
  line_to (SLIDE(1, 0.17), 0.0);
  line_to (0.1, 0.0);
  name ("eSpecial1b");

  NEW
  fprintf (stdout, "\n{ circle\n");
  {
    float scale = SLIDE (0.85, 0.2);
    vertex_scaled ('c', 0.11, 0.71, scale);
    vertex_scaled ('c', 0.29, 0.89, scale);
    vertex_scaled ('C', 0.50, 0.89, scale);
    vertex_scaled ('c', 0.71, 0.89, scale);
    vertex_scaled ('c', 0.89, 0.71, scale);
    vertex_scaled ('C', 0.89, 0.50, scale);
    vertex_scaled ('c', 0.89, 0.29, scale);
    vertex_scaled ('c', 0.71, 0.11, scale);
    vertex_scaled ('C', 0.50, 0.11, scale);
    vertex_scaled ('c', 0.29, 0.11, scale);
    vertex_scaled ('c', 0.11, 0.29, scale);
    vertex_scaled ('C', 0.11, 0.50, scale);
  }

  fprintf (stdout, "\n{ circlec\n");
  {
    float scale = SLIDE (0.85, 0.2);
    vertex_scaled2 ('c', 0.11, 0.71, scale);
    vertex_scaled2 ('c', 0.29, 0.89, scale);
    vertex_scaled2 ('C', 0.50, 0.89, scale);
    vertex_scaled2 ('c', 0.71, 0.89, scale);
    vertex_scaled2 ('c', 0.89, 0.71, scale);
    vertex_scaled2 ('C', 0.89, 0.50, scale);
    vertex_scaled2 ('c', 0.89, 0.29, scale);
    vertex_scaled2 ('c', 0.71, 0.11, scale);
    vertex_scaled2 ('C', 0.50, 0.11, scale);
    vertex_scaled2 ('c', 0.29, 0.11, scale);
    vertex_scaled2 ('c', 0.11, 0.29, scale);
    vertex_scaled2 ('C', 0.11, 0.50, scale);
  }

  fprintf (stdout, "\n{ circlecl\n");
  {
    float scale = SLIDE (0.85, 0.2);
    vertex_scaled3 ('c', 0.11, 0.71, scale);
    vertex_scaled3 ('c', 0.29, 0.89, scale);
    vertex_scaled3 ('C', 0.50, 0.89, scale);
    vertex_scaled3 ('c', 0.71, 0.89, scale);
    vertex_scaled3 ('c', 0.89, 0.71, scale);
    vertex_scaled3 ('C', 0.89, 0.50, scale);
    vertex_scaled3 ('c', 0.89, 0.29, scale);
    vertex_scaled3 ('c', 0.71, 0.11, scale);
    vertex_scaled3 ('C', 0.50, 0.11, scale);
    vertex_scaled3 ('c', 0.29, 0.11, scale);
    vertex_scaled3 ('c', 0.11, 0.29, scale);
    vertex_scaled3 ('C', 0.11, 0.50, scale);
  }
  return 0;
}
