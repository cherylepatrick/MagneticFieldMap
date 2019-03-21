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
    // I am going to stick with Steve's coordinates for this cos it is confusing otherwise
    int nx=std::stoi(GetBitBeforeComma(line));
    int ny=std::stoi(GetBitBeforeComma(line));
    int nz=std::stoi(GetBitBeforeComma(line));
    
    
    string xtitle="Centre to xwall";
    string ytitle="Along wires to vetos";
    string ztitle="Foil to main wall";
    
    std::vector<TH3D*> hComponents;
    for (int i=0;i<3;i++) // 3 components of the field
    {
      string comp="x"; if (i==1) comp="y";if (i==2)comp="z";
      string title="Field towards x wall"; if (i==1) title="Field along wires"; if (i==2) title="Field towards main wall";
      
      TH3D *field = new TH3D(title.c_str(),title.c_str(),nx,0,nx*0.02,ny,0,ny*0.02,nz,0,nz*0.02);
      for (int z=1;z<=nz;z++) //
      {
        for (int y=1;y<=ny;y++)
        {
          getline (*f,line);
          int ax=std::stoi(GetBitBeforeComma(line)); //index of the B vector's coordinates on X (ax=0), Y (ax=1) or Z (ax=2)
          int iy=std::stoi(GetBitBeforeComma(line)); //positional sampling index on the Y axis
          int iz=std::stoi(GetBitBeforeComma(line)); //positional sampling index on the Z axis
          for (int x=1;x<=nx;x++)
          {
            field->SetBinContent(x,y,z,std::stod(GetBitBeforeComma(line)));
          }
        }
      }
      
      // Print projections of each of the 3 field components, on each of the 3 2-d planes
      
      // Note that Steve's y field is the one along the wires i.e. the main component
      
      TCanvas *c=new TCanvas("c","c",900,600);
      
      TH2D *x_xy=(TH2D*)field->Project3D("xy"); // This will sum over z so divide by number of z bins to get an average field
      x_xy->Scale((1./nz));
      x_xy->GetYaxis()->SetTitle(xtitle.c_str());
      x_xy->GetXaxis()->SetTitle(ytitle.c_str());
      
      TH2D *x_yz=(TH2D*)field->Project3D("yz");
      x_yz->Scale((1./nx));
      x_yz->GetYaxis()->SetTitle(ytitle.c_str());
      x_yz->GetXaxis()->SetTitle(ztitle.c_str());
      
      TH2D *x_xz=(TH2D*)field->Project3D("xz");
      x_xz->Scale((1./ny));
      x_xz->GetYaxis()->SetTitle(xtitle.c_str());
      x_xz->GetXaxis()->SetTitle(ztitle.c_str());
      
      x_xy->Draw("COLZ");
      c->SaveAs((comp+"_xy.png").c_str());
      x_yz->Draw("COLZ");
      c->SaveAs((comp+"_yz.png").c_str());
      x_xz->Draw("COLZ");
      c->SaveAs((comp+"_xz.png").c_str());
      hComponents.push_back(field);
      
    }
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
