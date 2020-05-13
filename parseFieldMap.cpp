#include <iostream>
#include <fstream>
string GetBitBeforeComma(string& input);
TH1D *MakeFieldProjection(TH3D *plot3d, int ybin, int zbin, string title, int color);
void MakePlotSet(TH3D *totalfield, int ybin, string text);

string xtitle="C-section join to xwall (m)";
string ytitle="Vertical - centre to veto (m)";
string ztitle="Foil to main wall (m)";


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
    

    
    TCanvas *c=new TCanvas("c","c",900,600);
    
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
      gStyle->SetPalette(kBird);
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
    // 3d map of the field magnitude
    TH3D *totalfield = new TH3D("Field magnitude","Field magnitude",nx,0,nx*0.02,ny,0,ny*0.02,nz,0,nz*0.02);
    // Loop the bins and add the 3 field components at each location in quadrature
    for (int z=1;z<=nz;z++) //
    {
      for (int y=1;y<=ny;y++)
      {
        for (int x=1;x<=nx;x++)
        {
          double lengthsquare =0;
          for (int i=0;i<3;i++)
          {
            lengthsquare += pow(hComponents.at(i)->GetBinContent(x,y,z),2);
          }
          totalfield->SetBinContent(x,y,z,TMath::Sqrt(lengthsquare));
        }
      }
    }
    MakePlotSet(totalfield,1,"C-section join");
    MakePlotSet(totalfield,50,"1m from C-section join");
    MakePlotSet(totalfield,100,"2m from C-section join");
    MakePlotSet(totalfield,130,"Near Xwall");
    
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

TH1D *MakeFieldProjection(TH3D *plot3d, int ybin, int zbin, string title, int color)
{
  TH1D *plot= plot3d->ProjectionZ(title.c_str(),ybin,ybin,zbin,zbin);
  plot->Scale(0.001); // mGauss to Gauss
  plot->GetYaxis()->SetRangeUser(0,50);
  plot->GetXaxis()->SetTitle(ztitle.c_str());
  plot->GetYaxis()->SetTitle("Field magnitude (Gauss)");
  plot->SetTitle(title.c_str());
  plot->SetLineColor(color);
  plot->SetLineWidth(3);
  return plot;
}

void MakePlotSet(TH3D *totalfield, int ybin, string text)
{
  TCanvas *c=new TCanvas("c","c",900,600);
  // Take a 1d slice in our x (foil to main wall ) at y=0, z=0
  // In these coordinates we want a slice in z
  // And then we will take slices in z at various "y" values (our z, ie along the wires)
  
  TH1D *y0 =MakeFieldProjection(totalfield, ybin,1, "Centre of detector", kRed+2);
//  y0->Draw();
//  c->SaveAs((text+"_detector_centre.png").c_str());
  
  TH1D *y50 =MakeFieldProjection(totalfield, ybin,25, "50 cm above centre", kPink-2);
//  y50->Draw();
//  c->SaveAs((text+"_offset_50cm.png").c_str());
  
  TH1D *y1 =MakeFieldProjection(totalfield, ybin,50, "1m above centre", kMagenta+1);
//  y1->Draw();
//  c->SaveAs((text+"_offset_1_metre.png").c_str());
  
  TH1D *y150 =MakeFieldProjection(totalfield, ybin,75, "1.5m above centre", kViolet-3);
//  y150->Draw();
//  c->SaveAs((text+"_offset_150cm.png").c_str());
  
  TH1D *y18 =MakeFieldProjection(totalfield, ybin,90, "1.8m above centre", kBlue-7);
//  y18->Draw();
//  c->SaveAs((text+"_offset_1_8_metres.png").c_str());
  
  TH1D *yveto =MakeFieldProjection(totalfield, ybin,96, "Next to veto", kBlue+2);
//  yveto->Draw();
//  c->SaveAs((text+"_offset_veto.png").c_str());
  
  y0->SetTitle(("Field at different heights: "+text).c_str());
  y0->Draw();
  y50->Draw("SAME");
  y1->Draw("SAME");
  y150->Draw("SAME");
  y18->Draw("SAME");
  yveto->Draw("SAME");
  c->SetTitle(("Field at different heights: "+text).c_str());
  
  TLegend *leg=new TLegend(0.7,0.4,0.9,0.7); // Add a legend
  leg->AddEntry(y0, "Centre","l");
  leg->AddEntry(y50,"50cm from centre","l");
  leg->AddEntry(y1,"1m from centre","l");
  leg->AddEntry(y150,"1.5m from centre","l");
  leg->AddEntry(y18,"1.8m from centre","l");
  leg->AddEntry(yveto,"Next to veto","l");
  leg->Draw();
  
  c->SaveAs((text+"_offsets_all.png").c_str());
  
  // Select only the ybin in question and make a 2d plot
  TCanvas *c2=new TCanvas("c","c",600,900);
  TH3D *temp3d = (TH3D*)totalfield->Clone();
  temp3d->GetXaxis()->SetRange(ybin,ybin);
  TH2D *plot2d = (TH2D*)temp3d->Project3D("yz");
  plot2d->SetTitle(("Field (G) at "+text).c_str());
  plot2d->Scale(1./1000);
  plot2d->GetZaxis()->SetRangeUser(0,50);
  plot2d->GetXaxis()->SetTitle(ztitle.c_str());
  plot2d->GetYaxis()->SetTitle(ytitle.c_str());
  //c2->SetLogz();
  gStyle->SetPalette(kSunset);
  plot2d->Draw("COLZ");
  c2->SaveAs((text+"_2d.png").c_str());
}
