#include <QApplication>

#include "fitsviewer.h"

int main(int argc, char *argv[])
{
  QApplication qtApp(argc, argv);
  
  FitsViewer fitsv;
  
  fitsv.show();
  
  return qtApp.exec();
}