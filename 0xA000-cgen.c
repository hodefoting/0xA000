#include <gegl.h> 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define POTRACE_OPTS "-O 1.0 -t 4 "

const char uniglyph[8][8]=
{"        ",
 " 78mi^b ",
 " rUI  * ",
 " B= : ] ",
 "  B-T   ",
 "   $    ",
 "        ",
 "        "
};

const char palette[10][28]={
"                            ",
" 78m89 d^b \"^}      v /j~  ",
" 4UIO6 % * % *     /jlo u~  ",
" >Y R< q&p [&]    |D J   z? ",
" 4AWE6             FQdb !@  ",
" 12n23    7ik9 789|D qp: z? ",
" 789 7ik9 rUOy rty B= NT-_  ",
" 456 4gx6 fAEh fXh  B-_ $   ",
" 123 1ws3 1ws3 123   $      ",
"                            "};

const char mappings[17][4]={
{'7', '9', '3', '1'},
{'8', '6', '2', '4'},
{'m', '<', 'n', '>'},
{'i', 'y', 's', 'f'},
{'^', '*', '&', '%'},
{'b', 'p', 'q', 'd'},
{'r', 'k', 'h', 'w'},
{'U', 'O', 'E', 'A'},
{'I', 'R', 'W', 'Y'},
{'=', 'o', 'u', 'N'},
{':', 'Q', 'J', '!'},
{']', '[', '"', '}'},
{'B', '/', '~', '_'},
{'-', 'D', 'j', 'z'},
{'T', 'F', 'l', '@'},
{'$', '|', 'v', '?'},
{'\0' }
};

char *mem_read (char *start,
                char *linebuf,
                int  *len);

gint
main (gint    argc,
      gchar **argv)
{
  GeglNode *graph, *sink;
  GeglBuffer *buffer = NULL;
  char tmpbuf[2048];
  char *workdir;
  int units = 0;


  if (argc != 2)
    {
      g_print ("Usage: %s <input>\n", argv[0]);
      return 1;
    }

  gegl_init (&argc, &argv);

  workdir = g_dir_make_tmp (NULL, NULL);

  graph = gegl_graph (sink=gegl_node ("gegl:buffer-sink", "buffer", &buffer, NULL,
                           gegl_node ("gegl:load", "path", argv[1], NULL)));
  gegl_node_process (sink);
  g_object_unref (graph);
  if (!buffer)
    {
      g_print ("Failed to open %s\n", argv[1]);
      return 1;
    }

  units = gegl_buffer_get_width (buffer) / 8;

  int x, y;
  for (x = 1; x < 7; x ++)
    for (y = 1; y < 7; y ++)
      {
        int ascii = uniglyph[y][x];

        if (ascii != ' ')
          {
            char path[50];
            int u = x;
            int v = y;
            sprintf (path, "%s/c%X.png", workdir, ascii);

            graph = gegl_graph (sink=gegl_node ("gegl:save",
                                "path", path, NULL,
                                gegl_node ("gegl:crop", "x", u * units * 1.0, "y", v*units*1.0, "width", units*1.0, "height", units*1.0, NULL,
                                gegl_node ("gegl:buffer-source", "buffer", buffer, NULL))));
            gegl_node_process (sink);

            sprintf (path, "convert %s/c%X.png %s/c%X.pnm", workdir, ascii,  workdir, ascii);
            system (path);
            sprintf (path, "potrace %s/c%X.pnm %s -e -c  -M 0 --flat -q", workdir, ascii, POTRACE_OPTS);
            system (path);

            g_object_unref (buffer);  /* XXX: why is two unrefs needed here? */
            g_object_unref (graph);

            int mapping_no;
            for (mapping_no = 0; mappings[mapping_no][0] &&
                                 mappings[mapping_no][0] != ascii;
                                 mapping_no++);
            int variant;
            for (variant = 1; variant < 4; variant++)
              {
                int ascii2 = mappings[mapping_no][variant];
                sprintf (path, "%s/c%X.png", workdir, ascii2);

            graph = gegl_graph (sink=gegl_node ("gegl:save",
                                "path", path, NULL,
                                gegl_node ("gegl:crop", "x", u * units * 1.0, "y", v*units*1.0, "width", units*1.0, "height", units*1.0, NULL,
                                gegl_node ("gegl:rotate", "filter", "nohalo", "degrees", -90.0 * variant, "hard-edges", TRUE, "origin-x", units*(u+0.5), "origin-y", units*(v+0.5), NULL, 
                                gegl_node ("gegl:buffer-source", "buffer", buffer, NULL)))));
            gegl_node_process (sink);

            g_object_unref (buffer);  /* XXX: why is two unrefs needed here? */
            g_object_unref (graph);

            sprintf (path, "convert %s/c%X.png %s/c%X.pnm", workdir, ascii2, workdir, ascii2);
            system (path);


            sprintf (path, "potrace %s/c%X.pnm -e -c %s  -M 0 --flat -q", workdir, ascii2, POTRACE_OPTS);
            system (path);

              }
          }
      }

  int mapping_no;
  for (mapping_no = 0; mappings[mapping_no][0]; mapping_no++)
   {
    int variant;
    for (variant = 0; variant < 4; variant++)
      {
        int ascii2 = mappings[mapping_no][variant];
        char path[50];
        char *data;
        sprintf (path, "%s/c%X.eps", workdir, ascii2);
        g_file_get_contents (path, &data, NULL, NULL);

        char *p = data;
        int len;
        int started = 0;
        int x, y;
        int contour_no = 0;
        int coords = 0;


        do {
          char linebuf[1024];
          p = mem_read (p, linebuf, &len);
          if (p)
            {
              if (!started)
              {
                if (strstr (linebuf, "setgray"))
                  {
                    started = 1;
                  }
              }
              else
              {
                if (strstr (linebuf, "moveto"))
                  {
                    int u, v;
                    float X, Y;
                    if (contour_no)
                      printf ("Z\n");
                    else
                      printf ("\n{ c%X\n", ascii2);

                    contour_no++;

                    sscanf (linebuf, "%i %i moveto", &u, &v);
                    x = u; y = v;
                    X = x/1000.0;
                    Y = y/1000.0;
                    //printf ("M %i %i\n", u, v);
                    //printf ("M %f %f\n", X, Y);
                    coords++;
                  }
                else if (strstr (linebuf, "lineto"))
                  {
                    int u, v;
                    float X, Y;
                    sscanf (linebuf, "%i %i", &u, &v);
                    x += u; y += v;
                    X = x/1000.0;
                    Y = y/1000.0;
                    //printf ("L %i %i\n", x, y);
                    printf ("L %2.3f %2.3f\n", X, Y);
                    coords++;
                  }
                else if (strstr (linebuf, "curveto"))
                  {
                    int u0, v0, u1, v1, u2, v2;
                    float x0, y0, x1, y1, x2, y2;
                    sscanf (linebuf, "%i %i %i %i %i %i",
                        &u0, &v0,
                        &u1, &v1,
                        &u2, &v2);

                    u0 += x;
                    v0 += y;
                    u1 += x;
                    v1 += y;
                    u2 += x;
                    v2 += y;

                    x0 = u0/1000.0;
                    y0 = v0/1000.0;
                    x1 = u1/1000.0;
                    y1 = v1/1000.0;
                    x2 = u2/1000.0;
                    y2 = v2/1000.0;

                    //printf ("C %i %i %i %i %i %i\n", u0, v0, u1, v1, u2, v2);
                    printf ("c %2.3f %2.3f\n", x0, y0);
                    printf ("c %2.3f %2.3f\n", x1, y1);
                    printf ("C %2.3f %2.3f\n", x2, y2);

                    x = u2; y = v2;
                    coords++;
                  }

              }
            }
        } while (p);

        printf ("\n");
        if (coords)
          printf ("%c c%X\n", ascii2, ascii2);
        else
          printf ("%c blank\n", ascii2, ascii2);

        g_free (data);
      }
   }
  sprintf (tmpbuf, "rm -rf %s", workdir, workdir);
  system (tmpbuf);
  g_free (workdir);
  return 0;
}

char *mem_read (char *start,
                char *linebuf,
                int  *len)
{
    int  int_len;
    char *p = start;
    if (!len)
      len = &int_len;
    *len = 0;
    for (p = start; *p ;p++)
      {
        switch (*p)
         {
           case '#':
            while (*p && *p!='\n')p++;
            if (*p == '\n') p--;
             break;
           case '\n':
             {
               linebuf[*len]=0;
               return p+1;
             }
             break;
           case '\\':
             p++;
             if (*p)
               {
                 linebuf[(*len)++]=*p;
               }
             else
               p--;
             break;
           default:
             linebuf[(*len)++]=*p;
             break;
         }
      }

  return NULL;
}
