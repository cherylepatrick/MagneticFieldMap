#include <iostream>
#include <fstream>
string GetBitBeforeComma(string& input);

void parseFieldMap()
{
  gStyle->SetOptStat(0);
  ifstream *f = new ifstream();
  f->open("MapSmoothPlusDetail.csv");
  if (f->is_open())
  {
    string line;
    // read the lines of the file
    getline (*f,line);
    // First 3 things are the number of bins in x, y, and z
    int nx=std::stoi(GetBitBeforeComma(line));
    int ny=std::stoi(GetBitBeforeComma(line));
    int nz=std::stoi(GetBitBeforeComma(line));
    
    //TH3D *xfield = new TH3D("xfield","xfield",nx,ny,nz,0,nx,0,ny,0,nz);
    TH1D *xfield = new TH1D("x","x",nx,0,nx);
    TH2D *xyfield = new TH2D("xy","xy",nx,0,nx,ny,0,ny);
    cout<<nx<<":"<<ny<<":"<<nz<<endl;

    for (int y=1;y<=ny;y++)
    {
      getline (*f,line);
      int ax=std::stoi(GetBitBeforeComma(line)); //index of the B vector's coordinates on X (ax=0), Y (ax=1) or Z (ax=2)
      int iy=std::stoi(GetBitBeforeComma(line)); //positional sampling index on the Y axis
      int iz=std::stoi(GetBitBeforeComma(line)); //positional sampling index on the Z axis
      for (int x=01;x<=nx;x++)
      {
        xyfield->SetBinContent(x,y,std::stod(GetBitBeforeComma(line)));
      }
    }
    TCanvas *c=new TCanvas("c","c",900,600);
    xyfield->Draw("COLZ");
    c->SaveAs("temp.png");

    f->close();
  }
}

/**
 *  Return the part of the string that is before the first comma (trimmed of white space)
 *  Modify the input string to be whatever is AFTER the first comma
 *  If there is no comma, return the whole (trimmed) string and modify the input to zero-length string
 */
string GetBitBeforeComma(string& input)
{
  string output;
  int pos=input.find_first_of(',');
  if (pos <=0)
  {
  //  boost::trim(input);
    output=input;
    input="";
  }
  else
  {
    output=input.substr(0,pos);
    //boost::trim(output);
    input=input.substr(pos+1);
  }
  return output;
}
