/* compile and link: gcc <program file> -lncurses -lcurl */
 
#include <string.h>
#include <time.h>
#include <ncurses.h>
#include <curl/curl.h>
 
int main()
{
  char mesg1[] = "Hello World!";
  char mesg2[] = "Collecting data...";
  char head[] = " Rank |        AS name        | # inv. updates | \% of total ";
  //char str[160];
  int row, col;   // to store the number of rows and colums of the screen
  initscr();      // start the curses mode
  getmaxyx(stdscr, row, col);   // get the number of rows and columns
  mvprintw(row/2-1, (col-strlen(mesg1))/2, "%s", mesg1);  // 1st line
  mvprintw(row/2+1, (col-strlen(mesg2))/2, "%s", mesg2);  // 2nd line

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
    sscanf(buffer, "%d\t%s\t%21[^\n]", &num, org, name);
    asn[num][0] = strdup(org);
    asn[num][1] = strdup(name);
  }
  
  time_t last_elem_timestamp = 0;
  int pfx_valid = 0;
  int pfx_invalid = 0;
  int pfx_notfound = 0;
  int chart[5][2] = {{0,0},{0,0},{0,0},{0,0},{0,0}};
  int chart_length = sizeof(chart)/sizeof(chart[0]);
  int i = 0;

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
    mvprintw(row/2-3, (col-strlen(head))/2, "%s\n", head);
    if(pfx_invalid > 0
      && chart[1][1] > chart[2][1]
      && chart[2][1] > chart[3][1]
    ) {
      for(i=0; i<chart_length; i++) {
        mvprintw(row/2-2+i, (col-strlen(head))/2, "%5d | %-21s | %14d | %10d",
          i+1, asn[(chart[i][0])][1], chart[i][1], chart[i][1]*100/pfx_invalid);
      }
    }
    mvprintw(1, 2, "Processed updates:"); 
    mvprintw(2, 2, "%18d", pfx_valid + pfx_invalid + pfx_notfound);
    /*
    mvprintw(1, col-26, "Last update:");
    mvprintw(2, col-26, "%s", asctime(localtime(&last_elem_timestamp)));
    */
    last_elem_timestamp = time(NULL) - time(NULL) % 300;
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
