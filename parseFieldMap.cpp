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
    
    TH3D *xfield = new TH3D("xfield","xfield",nx,0,nx,ny,0,ny,nz,0,nz);
    //TH1D *xfield = new TH1D("x","x",nx,0,nx);
    //TH2D *xyfield = new TH2D("xy","xy",nx,0,nx,ny,0,ny);
    cout<<nx<<":"<<ny<<":"<<nz<<endl;
    for (int z=1;z<=nz;z++)
    {
      for (int y=1;y<=ny;y++)
      {
        getline (*f,line);
        int ax=std::stoi(GetBitBeforeComma(line)); //index of the B vector's coordinates on X (ax=0), Y (ax=1) or Z (ax=2)
        int iy=std::stoi(GetBitBeforeComma(line)); //positional sampling index on the Y axis
        int iz=std::stoi(GetBitBeforeComma(line)); //positional sampling index on the Z axis
        for (int x=1;x<=nx;x++)
        {
          xfield->SetBinContent(x,y,z,std::stod(GetBitBeforeComma(line)));
        }
      }
    }
    TCanvas *c=new TCanvas("c","c",900,600);
    //xyfield->Draw("COLZ");
    TH2D *x_xy=(TH2D*)xfield->Project3D("xy"); // This will sum over z so divide by number of z bins to get an average field
    x_xy->Scale((1./nz));
    TH2D *x_yz=(TH2D*)xfield->Project3D("yz");
    x_yz->Scale((1./nx));
    TH2D *x_xz=(TH2D*)xfield->Project3D("xz");
    x_xz->Scale((1./ny));
    
    x_xy->Draw("COLZ");
    c->SaveAs("x_xy.png");
    x_yz->Draw("COLZ");
    c->SaveAs("x_yz.png");
    x_xz->Draw("COLZ");
    c->SaveAs("x_xz.png");

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
