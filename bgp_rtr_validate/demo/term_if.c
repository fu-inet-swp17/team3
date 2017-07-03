/* compile and link: gcc <program file> -lncurses -lcurl */
 
#include <string.h>
#include <time.h>
#include <ncurses.h>
#include <curl/curl.h>

time_t t = 0;
int chart_pos = 0;
int chart_val[18] = {0};

/* Not consistent for some reason */
void print_seal(int row, int col) {
  getmaxyx(stdscr, row, col);
  char mesg[48];
  strcpy(mesg, "UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU");
  mvprintw(row/2-14, (col-strlen(mesg))/2, "%s", mesg);
  strcpy(mesg, "U     -°:°           -::*=UUUUUU°:      U");
  mvprintw(row/2-13, (col-strlen(mesg))/2, "%s", mesg);
  strcpy(mesg, " U     -°--°:          +UUUUUU°UUUUU-     U");
  mvprintw(row/2-12, (col-strlen(mesg))/2, "%s", mesg);
  strcpy(mesg, "    U      ::--°-        °°°°.-:=UUUUUUUU     U");
  mvprintw(row/2-11, (col-strlen(mesg))/2, "%s", mesg);
  strcpy(mesg, "   U     -°-:°          :°°°:+UUUUUUUU-     U");
  mvprintw(row/2-10, (col-strlen(mesg))/2, "%s", mesg);
  strcpy(mesg, " U     -°:°°            °UUUUUUUUUU:     U");
  mvprintw(row/2-9, (col-strlen(mesg))/2, "%s", mesg);
  strcpy(mesg, "U      =-°=           -°UUUUUUUUU+       U");
  mvprintw(row/2-8, (col-strlen(mesg))/2, "%s", mesg);
  strcpy(mesg, "U      -:°U°        :UUUUUUUUUUUU°      U");
  mvprintw(row/2-7, (col-strlen(mesg))/2, "%s", mesg);
  strcpy(mesg, "U    -°UUUUUUU=**+::°UUUUUUUUUUUUU+      U");
  mvprintw(row/2-6, (col-strlen(mesg))/2, "%s", mesg);
  strcpy(mesg, "U      =UUU+=UUUUUUUUUUUUUUUUUUUUUU:       U");
  mvprintw(row/2-5, (col-strlen(mesg))/2, "%s", mesg);
  strcpy(mesg, "U      =UUU°°=UUUUUUUUUUUUUUUUUUUU       U");
  mvprintw(row/2-4, (col-strlen(mesg))/2, "%s", mesg);
  strcpy(mesg, "U       ::=UUUUUUUUUUUUUUUUUUUUUUU*        U");
  mvprintw(row/2-3, (col-strlen(mesg))/2, "%s", mesg);
  strcpy(mesg, "U              +++**=UUUUUUUUUUUUU=        U");
  mvprintw(row/2-2, (col-strlen(mesg))/2, "%s", mesg);
  strcpy(mesg, "UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU");
  mvprintw(row/2-1, (col-strlen(mesg))/2, "%s", mesg);
  strcpy(mesg, "U-    UUUUUUUUUUUUU      UUUUUUUUUUUUU    -U");
  mvprintw(row/2, (col-strlen(mesg))/2, "%s", mesg);
  strcpy(mesg, "U    U::-++:-:-..U      U+::+::.-...U    U");
  mvprintw(row/2+1, (col-strlen(mesg))/2, "%s", mesg);
  strcpy(mesg, "U-   U-+VERITAS:+U      U.IVSTITIA.+U   -U");
  mvprintw(row/2+2, (col-strlen(mesg))/2, "%s", mesg);
  strcpy(mesg, "U-  UUUUUUUUUUUUU      UUUUUUUUUUUUU  -U");
  mvprintw(row/2+3, (col-strlen(mesg))/2, "%s", mesg);
  strcpy(mesg, "U+                                  +U");
  mvprintw(row/2+4, (col-strlen(mesg))/2, "%s", mesg);
  strcpy(mesg, "U+          UUUUUUUUUUUU          +U");
  mvprintw(row/2+5, (col-strlen(mesg))/2, "%s", mesg);
  strcpy(mesg, " U°        U.*::*++*::U         °U");
  mvprintw(row/2+6, (col-strlen(mesg))/2, "%s", mesg);
  strcpy(mesg, "U+       U*LIBERTAS-U       +U");
  mvprintw(row/2+7, (col-strlen(mesg))/2, "%s", mesg);
  strcpy(mesg, "  U°     UUUUUUUUUUUU     °U");
  mvprintw(row/2+8, (col-strlen(mesg))/2, "%s", mesg);
  strcpy(mesg, "UU:                :UU");
  mvprintw(row/2+9, (col-strlen(mesg))/2, "%s", mesg);
  strcpy(mesg, " UUU°         °UUU");
  mvprintw(row/2+10, (col-strlen(mesg))/2, "%s", mesg);
  strcpy(mesg, "  °UUUUUUUU°");
  mvprintw(row/2+11, (col-strlen(mesg))/2, "%s", mesg);
}

void print_bar_chart(int row, int col, int valid, int total, time_t timestamp)
{
  getmaxyx(stdscr, row, col);
  char mesg[80];
  int i, j;
  strcpy(mesg, "^");
  mvprintw(row/2-10, (col-strlen(mesg))/2-28, "%s", mesg);
  strcpy(mesg, "\% valid |");
  mvprintw(row/2-9, (col-strlen(mesg))/2-32, "%s", mesg);
  strcpy(mesg, "updates |");
  mvprintw(row/2-8, (col-strlen(mesg))/2-32, "%s", mesg);
  strcpy(mesg, "|");
  for(i=-7; i<1; i++) {
    mvprintw(row/2+i, (col-strlen(mesg))/2-28, "%s", mesg);
  }
  strcpy(mesg, "|=====|=====|=====|=====|=====|=====|=====|=====|=====>");
  mvprintw(row/2+1, (col-strlen(mesg))/2-1, "%s", mesg);
  strcpy(mesg, "10    20    30    40    50    60    70    80    time (min)");
  mvprintw(row/2+2, (col-strlen(mesg))/2+6, "%s", mesg);

  if((chart_pos < 17) && (valid > 0)) {
    if(timestamp > t) {
      t = timestamp;
      chart_pos++;
    }
    chart_val[chart_pos] = valid*100/total;
    for(i=0; i<=chart_pos; i++) {
      mvprintw(row/2-chart_val[i]/2, (col)/2-27+i*3, "%2d", chart_val[i]);
      for(j=1; j<=chart_val[i]/2; j++) {
        mvprintw(row/2-chart_val[i]/2+j, (col)/2-27+i*3, "%s", "**");
      }
    }
  }
}
 
int main()
{
  //char mesg1[] = "Hello World!";
  char mesg2[] = "Collecting data...";
  char head[] =
    "| Rank |            AS name            | Σ inv. upd | \% of total inv. |";
  //char str[160];
  int row, col;   // to store the number of rows and colums of the screen
  initscr();      // start the curses mode
  getmaxyx(stdscr, row, col);   // get the number of rows and columns
  //mvprintw(row/2-1, (col-strlen(mesg1))/2, "%s", mesg1);  // 1st line
  print_seal(row, col);
  mvprintw(row/2+13, (col-strlen(mesg2))/2, "%s", mesg2);  // 2nd line

  FILE *fp;
  char asn_file[FILENAME_MAX] = "asn.txt";

  /* Don't download the file if it already exists */
  if((fopen(asn_file, "r")) == NULL) {
    CURL *curl;
    char *url = "http://www.potaroo.net/bgp/iana/asn-ctl.txt";
    curl = curl_easy_init();
    if(!curl) {
      endwin();
      fprintf(stderr, "\tError while downloading ASN file\n");
      return -1;
    }
    //CURLcode res;
    fp = fopen(asn_file, "wb");
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    //res = curl_easy_perform(curl);
    curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    fclose(fp);
  }

  fp = fopen(asn_file, "r");
  const char *asn[400000][2];
  char buffer[255];
  int num;
  char org[15];
  char name[127];

  /* Read data from ASN file */
  while(fgets(buffer, sizeof buffer, fp) != NULL) {
    sscanf(buffer, "%d\t%s\t%29[^\n]", &num, org, name);
    asn[num][0] = strdup(org);
    asn[num][1] = strdup(name);
  }
  
  time_t last_elem_timestamp = 0;
  t = time(NULL) - time(NULL) % 300;
  int pfx_valid = 0;
  int pfx_invalid = 0;
  int pfx_notfound = 0;
  int chart[5][2] = {{0,0},{0,0},{0,0},{0,0},{0,0}};
  int chart_length = sizeof(chart)/sizeof(chart[0]);
  int i, j;   // loop variables
  int percent = 0;
  char percent_bar[20];

  for(;;) {
    /*
    if(scanw("%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d",
      &last_elem_timestamp,
    */
    if(scanw("%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d",
      &pfx_valid,
      &pfx_invalid,
      &pfx_notfound,
      &chart[0][0],
      &chart[0][1],
      &chart[1][0],
      &chart[1][1],
      &chart[2][0],
      &chart[2][1],
      &chart[3][0],
      &chart[3][1],
      &chart[4][0],
      &chart[4][1]
    /*
    ) != 14) {
    */
    ) != 13) {
      endwin();
      fprintf(stderr, "\tError while parsing input\n");
      return -1;
    }
    erase();  // clear screen
    
    last_elem_timestamp = time(NULL) - time(NULL) % 300;
    
    print_bar_chart(
      row, col,
      pfx_valid, pfx_valid + pfx_invalid + pfx_notfound,
      last_elem_timestamp
    );
    
    mvprintw(row/2+5, (col-strlen(head))/2, "%s", head);
    if(
      pfx_invalid > 0
      //&& chart[1][1] > chart[2][1]
      //&& chart[2][1] > chart[3][1]
    ) {
      for(i=0; i<chart_length; i++) {
		percent = chart[i][1]*100/pfx_invalid;
		strcpy(percent_bar, "| ");
		for(j=0; j<percent-percent%10; j+=10) {
          strcat(percent_bar, "*");
        }
        for(j=j; j<90; j+=10) {
          strcat(percent_bar, " ");
        }
        mvprintw(
          row/2+6+i,
          (col-strlen(head))/2,
          "%s %4d | %-29s | %10d | %3d %s %s",
          "|",
          i+1,                    // Rank (1-5)
          asn[(chart[i][0])][1],  // AS name
          chart[i][1],            // inv. updates of AS
          percent,
          percent_bar,
          "|"
        );
      }
    }
    mvprintw(1, 2, "Processed updates:"); 
    mvprintw(2, 2, "%18d", pfx_valid + pfx_invalid + pfx_notfound);
    /*
    mvprintw(1, col-26, "Last update:");
    mvprintw(2, col-26, "%s", asctime(localtime(&last_elem_timestamp)));
    */
    mvprintw(1, col-26, "Last update (est.):");
    mvprintw(2, col-26, "%s", asctime(localtime(&last_elem_timestamp)));
    mvprintw(row-1, 0, "");   // go to bottom
    beep();
    flash();
  }
  fclose(fp);
  getch();
  endwin();
  return 0;
}
