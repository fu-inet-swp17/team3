/* compile and link: gcc <program file> -lncurses */

#include <ncurses.h> 
#include <string.h>
#include <time.h>
 
int main()
{
  char mesg1[] = "Hello World!";
  char mesg2[] = "Collecting data...";
  char head[] = " Rank | AS no. | # inv. updates | \% of total ";
  //char str[160];
  int row, col;   // to store the number of rows and colums of the screen
  initscr();      // start the curses mode
  getmaxyx(stdscr, row, col);   // get the number of rows and columns
  mvprintw(row/2-1, (col-strlen(mesg1))/2, "%s", mesg1);  // 1st line
  mvprintw(row/2+1, (col-strlen(mesg2))/2, "%s", mesg2);  // 2nd line
  
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
        mvprintw(row/2-2+i, (col-strlen(head))/2, "%5d | %6d | %14d | %10d",
          i+1, chart[i][0], chart[i][1], chart[i][1]*100/pfx_invalid);
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
  getch();
  endwin();
  return 0;
}
