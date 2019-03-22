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
    // Take a 1d slice in our x (foil to main wall ) at y=0, z=0
    // In these coordinates we want a slice in z
    // And then we will take slices in z at various "y" values (our z, ie along the wires)
    
    TH1D *y0 = totalfield->ProjectionZ("centre",1,1,1,1);
    y0->GetYaxis()->SetRangeUser(0,50000);
    y0->GetXaxis()->SetTitle(ztitle.c_str());
    y0->SetTitle("Centre of detector");
    y0->SetLineColor(kRed+2);
    y0->SetLineWidth(3);
    y0->Draw();
    c->SaveAs("detector_centre.png");
    
    TH1D *y50 = totalfield->ProjectionZ("HalfMetre",1,1,25,25);
    y50->GetYaxis()->SetRangeUser(0,50000);
    y50->GetXaxis()->SetTitle(ztitle.c_str());
    y50->SetTitle("50 cm above centre");
    y50->SetLineColor(kPink-2);
    y50->SetLineWidth(3);
    y50->Draw();
    c->SaveAs("offset_50cm.png");
    
    TH1D *y1 = totalfield->ProjectionZ("Metre1",1,1,50,50);
    y1->GetYaxis()->SetRangeUser(0,50000);
    y1->GetXaxis()->SetTitle(ztitle.c_str());
    y1->SetTitle("1 metre above centre");
    y1->SetLineColor(kMagenta+1);
    y1->SetLineWidth(3);
    y1->Draw();
    c->SaveAs("offset_1_metre.png");
    
    TH1D *y150 = totalfield->ProjectionZ("OneAndHalfMetre",1,1,75,75);
    y150->GetYaxis()->SetRangeUser(0,50000);
    y150->GetXaxis()->SetTitle(ztitle.c_str());
    y150->SetTitle("150 cm above centre");
    y150->SetLineColor(kViolet-3);
    y150->SetLineWidth(3);
    y150->Draw();
    c->SaveAs("offset_150cm.png");
    
    TH1D *y18 = totalfield->ProjectionZ("Cm180",1,1,90,90);
    y18->GetYaxis()->SetRangeUser(0,50000);
    y18->GetXaxis()->SetTitle(ztitle.c_str());
    y18->SetTitle("1.8 metres above centre (very close to veto)");
    y18->SetLineColor(kBlue-7);
    y18->SetLineWidth(3);
    y18->Draw();
    c->SaveAs("offset_1_8_metres.png");
    
    TH1D *yveto = totalfield->ProjectionZ("ByVeto",1,1,96,96);
    yveto->GetYaxis()->SetRangeUser(0,50000);
    yveto->GetXaxis()->SetTitle(ztitle.c_str());
    yveto->SetTitle("Right next to veto");
    yveto->SetLineColor(kBlue+1);
    yveto->SetLineWidth(3);
    yveto->Draw();
    c->SaveAs("offset_veto.png");
    
    y0->SetTitle("Field magnitude for various heights");
    y0->Draw();
    y50->Draw("SAME");
    y1->Draw("SAME");
    y150->Draw("SAME");
    y18->Draw("SAME");
    yveto->Draw("SAME");
    c->SetTitle("Field at different heights");
    
    TLegend *leg=new TLegend(0.6,0.4,0.85,0.7); // Add a legend
    leg->AddEntry(y0, "Centre","l");
    leg->AddEntry(y50,"50cm from centre","l");
    leg->AddEntry(y1,"1m from centre","l");
    leg->AddEntry(y150,"1.5m from centre","l");
    leg->AddEntry(y18,"1.8m from centre","l");
    leg->AddEntry(yveto,"Next to veto","l");
    leg->Draw();
    
    c->SaveAs("offsets_all.png");
    
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
