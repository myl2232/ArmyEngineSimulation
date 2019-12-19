#include "Canvas.h"
#include "LineEntity.h"
#include "TextStyle.h"
#include "AllowWindowsPlatformTypes.h"
#include "Windows.h"
#include "HideWindowsPlatformTypes.h"
enum KnownVariable {
	ANGBASE,
	ANGDIR,
	ATTMODE,
	AUNITS,
	AUPREC,
	CECOLOR,
	CELTSCALE,
	CHAMFERA,
	CHAMFERB,
	CHAMFERC,
	CHAMFERD,
	CLAYER,
	CMLJUST,
	CMLSCALE,
	/** Decimal places in angular dimensions */
	DIMADEC,
	DIMALT,
	DIMALTD,
	DIMALTF,
	DIMALTRND,
	DIMALTTD,
	DIMALTTZ,
	DIMALTU,
	DIMALTZ,
	DIMAPOST,
	DIMASZ,
	DIMATFIT,
	/** Formatting of angular dimensions */
	DIMAUNIT,
	/** Controls display of leading / trailing zeros for angular dimensions */
	DIMAZIN,
	DIMBLK,
	DIMBLK1,
	DIMBLK2,
	DIMCEN,
	DIMCLRD,
	DIMCLRE,
	DIMCLRT,
	/** Decimal places in linear dimensions */
	DIMDEC,
	DIMDLE,
	DIMDLI,
	/** Decimal separator in dimensions */
	DIMDSEP,
	DIMEXE,
	DIMEXO,
	DIMFRAC,
	/** Distance between dimension text and dimension lines, negative for box */
	DIMGAP,
	/** Vertical position of dimension label */
	DIMJUST,
	DIMLDRBLK,
	DIMLFAC,
	DIMLIM,
	/** Formatting of linear dimensions */
	DIMLUNIT,
	DIMLWD,
	DIMLWE,
	DIMPOST,
	DIMRND,
	DIMSAH,
	DIMSCALE,
	DIMSD1,
	DIMSD2,
	DIMSE1,
	DIMSE2,
	DIMSOXD,
	/** Vertical position of dimension label */
	DIMTAD,
	DIMTDEC,
	DIMTFAC,
	DIMTIH,
	DIMTIX,
	DIMTM,
	DIMTOFL,
	DIMTOH,
	DIMTOL,
	DIMTOLJ,
	DIMTP,
	/** Archtick size or 0 for arrows */
	DIMTSZ,
	DIMTVP,
	DIMTXSTY,
	/** Dimension text size */
	DIMTXT,
	DIMTZIN,
	DIMUPT,
	/** Controls display of leading / trailing zeros for linear dimensions */
	DIMZIN,
	DISPSILH,
	DWGCODEPAGE,
	DRAWORDERCTL,
	ELEVATION,
	EXTMAX,
	EXTMIN,
	FACETRES,
	FILLETRAD,
	FILLMODE,
	INSBASE,
	INSUNITS,
	ISOLINES,
	LIMCHECK,
	LIMMAX,
	LIMMIN,
	LTSCALE,
	LUNITS,
	LUPREC,
	MAXACTVP,
	MEASUREMENT,
	MIRRTEXT,
	ORTHOMODE,
	/* Point display mode */
	PDMODE,
	/* Point display size */
	PDSIZE,
	PELEVATION,
	PELLIPSE,
	PEXTMAX,
	PEXTMIN,
	PINSBASE,
	PLIMCHECK,
	PLIMMAX,
	PLIMMIN,
	PLINEGEN,
	PLINEWID,
	PROXYGRAPHICS,
	PSLTSCALE,
	PUCSNAME,
	PUCSORG,
	PUCSXDIR,
	PUCSYDIR,
	QTEXTMODE,
	REGENMODE,
	SHADEDGE,
	SHADEDIF,
	SKETCHINC,
	SKPOLY,
	SPLFRAME,
	SPLINESEGS,
	SPLINETYPE,
	SURFTAB1,
	SURFTAB2,
	SURFTYPE,
	SURFU,
	SURFV,
	TEXTQLTY,
	TEXTSIZE,
	TEXTSTYLE,
	THICKNESS,
	TILEMODE,
	TRACEWID,
	TREEDEPTH,
	UCSNAME,
	UCSORG,
	UCSXDIR,
	UCSYDIR,
	UNITMODE,
	USERI1,
	USERI2,
	USERI3,
	USERI4,
	USERI5,
	USERR1,
	USERR2,
	USERR3,
	USERR4,
	USERR5,
	USRTIMER,
	VISRETAIN,
	WORLDVIEW,
	MaxKnownVariable = WORLDVIEW,
	INVALID = -1
};
int getCodeForVariable(KnownVariable v) {
	switch (v) {
		//    case 3DDWFPREC:
		//        return 40;
		//    case ACADMAINTVER:
		//        return 70;
		//    case ACADVER:
		//        return 1;
	case ANGBASE:
		return 50;
	case ANGDIR:
		return 70;
	case ATTMODE:
		return 70;
	case AUNITS:
		return 70;
	case AUPREC:
		return 70;
		//    case CAMERADISPLAY:
		//        return 290;
		//    case CAMERAHEIGHT:
		//        return 40;
	case CECOLOR:
		return 62;
	case CELTSCALE:
		return 40;
		//    case CELTYPE:
		//        return 6;
		//    case CELWEIGHT:
		//        return 370;
		//    case CEPSNTYPE:
		//        return 380;
	case CHAMFERA:
		return 40;
	case CHAMFERB:
		return 40;
	case CHAMFERC:
		return 40;
	case CHAMFERD:
		return 40;
		//    case CLAYER:
		//        return 8;
		//    case CMATERIAL:
		//        return 347;
	case CMLJUST:
		return 70;
	case CMLSCALE:
		return 40;
		//    case CMLSTYLE:
		//        return 2;
		//    case CSHADOW:
		//        return 280;
		//    case DGNFRAME:
		//        return 280;
	case DIMADEC:
		return 70;
	case DIMALT:
		return 70;
	case DIMALTD:
		return 70;
	case DIMALTF:
		return 40;
	case DIMALTRND:
		return 40;
	case DIMALTTD:
		return 70;
	case DIMALTTZ:
		return 70;
	case DIMALTU:
		return 70;
	case DIMALTZ:
		return 70;
	case DIMAPOST:
		return 1;
		//    case DIMARCSYM:
		//        return 70;
		//    case DIMASO:
		//        return 70;
		//    case DIMASSOC:
		//        return 280;
	case DIMASZ:
		return 40;
	case DIMATFIT:
		return 70;
	case DIMAUNIT:
		return 70;
	case DIMAZIN:
		return 70;
	case DIMBLK:
		return 1;
	case DIMBLK1:
		return 1;
	case DIMBLK2:
		return 1;
	case DIMCEN:
		return 40;
	case DIMCLRD:
		return 70;
	case DIMCLRE:
		return 70;
	case DIMCLRT:
		return 70;
	case DIMDEC:
		return 70;
	case DIMDLE:
		return 40;
	case DIMDLI:
		return 40;
	case DIMDSEP:
		return 70;
	case DIMEXE:
		return 40;
	case DIMEXO:
		return 40;
	case DIMFRAC:
		return 70;
		//    case DIMFXL:
		//        return 40;
		//    case DIMFXLON:
		//        return 70;
	case DIMGAP:
		return 40;
		//    case DIMJOGANG:
		//        return 40;
	case DIMJUST:
		return 70;
	case DIMLDRBLK:
		return 1;
	case DIMLFAC:
		return 40;
	case DIMLIM:
		return 70;
		//    case DIMLTEX1:
		//        return 6;
		//    case DIMLTEX2:
		//        return 6;
		//    case DIMLTYPE:
		//        return 6;
	case DIMLUNIT:
		return 70;
	case DIMLWD:
		return 70;
	case DIMLWE:
		return 70;
	case DIMPOST:
		return 1;
	case DIMRND:
		return 40;
	case DIMSAH:
		return 70;
	case DIMSCALE:
		return 40;
	case DIMSD1:
		return 70;
	case DIMSD2:
		return 70;
	case DIMSE1:
		return 70;
	case DIMSE2:
		return 70;
		//    case DIMSHO:
		//        return 70;
	case DIMSOXD:
		return 70;
		//    case DIMSTYLE:
		//        return 2;
	case DIMTAD:
		return 70;
	case DIMTDEC:
		return 70;
	case DIMTFAC:
		return 40;
		//    case DIMTFILL:
		//        return 70;
		//    case DIMTFILLCLR:
		//        return 70;
	case DIMTIH:
		return 70;
	case DIMTIX:
		return 70;
	case DIMTM:
		return 40;
		//    case DIMTMOVE:
		//        return 70;
	case DIMTOFL:
		return 70;
	case DIMTOH:
		return 70;
	case DIMTOL:
		return 70;
	case DIMTOLJ:
		return 70;
	case DIMTP:
		return 40;
	case DIMTSZ:
		return 40;
	case DIMTVP:
		return 40;
	case DIMTXSTY:
		return 7;
	case DIMTXT:
		return 40;
		//    case DIMTXTDIRECTION:
		//        return 70;
	case DIMTZIN:
		return 70;
	case DIMUPT:
		return 70;
	case DIMZIN:
		return 70;
	case DISPSILH:
		return 70;
		//    case DWFFRAME:
		//        return 280;
	case DWGCODEPAGE:
		return 3;
	case ELEVATION:
		return 40;
		//    case ENDCAPS:
		//        return 280;
	case EXTMAX:
		return 10;
	case EXTMIN:
		return 10;
		//    case EXTNAMES:
		//        return 290;
	case FILLETRAD:
		return 40;
	case FILLMODE:
		return 70;
		//    case FINGERPRINTGUID:
		//        return 2;
		//    case HALOGAP:
		//        return 280;
		//    case HANDSEED:
		//        return 5;
		//    case HIDETEXT:
		//        return 280;
		//    case HYPERLINKBASE:
		//        return 1;
		//    case INDEXCTL:
		//        return 280;
	case INSBASE:
		return 10;
	case INSUNITS:
		return 70;
		//    case INTERFERECOLOR:
		//        return 62;
		//    case INTERSECTIONCOLOR:
		//        return 70;
		//    case INTERSECTIONDISPLAY:
		//        return 280;
		//    case JOINSTYLE:
		//        return 280;
		//    case LATITUDE:
		//        return 40;
		//    case LENSLENGTH:
		//        return 40;
		//    case LIGHTGLYPHDISPLAY:
		//        return 280;
	case LIMCHECK:
		return 70;
	case LIMMAX:
		return 10;
	case LIMMIN:
		return 10;
		//    case LOFTANG1:
		//        return 40;
		//    case LOFTANG2:
		//        return 40;
		//    case LOFTMAG1:
		//        return 40;
		//    case LOFTMAG2:
		//        return 40;
		//    case LOFTNORMALS:
		//        return 280;
		//    case LOFTPARAM:
		//        return 70;
		//    case LONGITUDE:
		//        return 40;
	case LTSCALE:
		return 40;
	case LUNITS:
		return 70;
	case LUPREC:
		return 70;
		//    case LWDISPLAY:
		//        return 290;
	case MAXACTVP:
		return 70;
	case MEASUREMENT:
		return 70;
		//    case MENU:
		//        return 1;
	case MIRRTEXT:
		return 70;
		//    case NORTHDIRECTION:
		//        return 40;
		//    case OBSCOLOR:
		//        return 70;
		//    case OBSLTYPE:
		//        return 280;
		//    case OLESTARTUP:
		//        return 290;
	case ORTHOMODE:
		return 70;
	case PDMODE:
		return 70;
	case PDSIZE:
		return 40;
	case PELEVATION:
		return 40;
	case PEXTMAX:
		return 10;
	case PEXTMIN:
		return 10;
	case PINSBASE:
		return 10;
	case PLIMCHECK:
		return 70;
	case PLIMMAX:
		return 10;
	case PLIMMIN:
		return 10;
	case PLINEGEN:
		return 70;
	case PLINEWID:
		return 40;
		//    case PROJECTNAME:
		//        return 1;
	case PROXYGRAPHICS:
		return 70;
	case PSLTSCALE:
		return 70;
		//    case PSOLHEIGHT:
		//        return 40;
		//    case PSOLWIDTH:
		//        return 40;
		//    case PSTYLEMODE:
		//        return 290;
		//    case PSVPSCALE:
		//        return 40;
		//    case PUCSBASE:
		//        return 2;
	case PUCSNAME:
		return 2;
	case PUCSORG:
		return 10;
		//    case PUCSORGBACK:
		//        return 10;
		//    case PUCSORGBOTTOM:
		//        return 10;
		//    case PUCSORGFRONT:
		//        return 10;
		//    case PUCSORGLEFT:
		//        return 10;
		//    case PUCSORGRIGHT:
		//        return 10;
		//    case PUCSORGTOP:
		//        return 10;
		//    case PUCSORTHOREF:
		//        return 2;
		//    case PUCSORTHOVIEW:
		//        return 70;
	case PUCSXDIR:
		return 10;
	case PUCSYDIR:
		return 10;
	case QTEXTMODE:
		return 70;
		//    case REALWORLDSCALE:
		//        return 290;
	case REGENMODE:
		return 70;
	case SHADEDGE:
		return 70;
	case SHADEDIF:
		return 70;
		//    case SHADOWPLANELOCATION:
		//        return 40;
		//    case SHOWHIST:
		//        return 280;
	case SKETCHINC:
		return 40;
	case SKPOLY:
		return 70;
		//    case SOLIDHIST:
		//        return 280;
		//    case SORTENTS:
		//        return 280;
	case SPLFRAME:
		return 70;
	case SPLINESEGS:
		return 70;
	case SPLINETYPE:
		return 70;
		//    case STEPSIZE:
		//        return 40;
		//    case STEPSPERSEC:
		//        return 40;
		//    case STYLESHEET:
		//        return 1;
	case SURFTAB1:
		return 70;
	case SURFTAB2:
		return 70;
	case SURFTYPE:
		return 70;
	case SURFU:
		return 70;
	case SURFV:
		return 70;
		//    case TDCREATE:
		//        return 40;
		//    case TDINDWG:
		//        return 40;
		//    case TDUCREATE:
		//        return 40;
		//    case TDUPDATE:
		//        return 40;
		//    case TDUSRTIMER:
		//        return 40;
		//    case TDUUPDATE:
		//        return 40;
	case TEXTSIZE:
		return 40;
	case TEXTSTYLE:
		return 7;
	case THICKNESS:
		return 40;
	case TILEMODE:
		return 70;
		//    case TILEMODELIGHTSYNCH:
		//        return 280;
		//    case TIMEZONE:
		//        return 70;
	case TRACEWID:
		return 40;
	case TREEDEPTH:
		return 70;
		//    case UCSBASE:
		//        return 2;
	case UCSNAME:
		return 2;
	case UCSORG:
		return 10;
		//    case UCSORGBACK:
		//        return 10;
		//    case UCSORGBOTTOM:
		//        return 10;
		//    case UCSORGFRONT:
		//        return 10;
		//    case UCSORGLEFT:
		//        return 10;
		//    case UCSORGRIGHT:
		//        return 10;
		//    case UCSORGTOP:
		//        return 10;
		//    case UCSORTHOREF:
		//        return 2;
		//    case UCSORTHOVIEW:
		//        return 70;
	case UCSXDIR:
		return 10;
	case UCSYDIR:
		return 10;
	case UNITMODE:
		return 70;
	case USERI1:
		return 70;
	case USERI2:
		return 70;
	case USERI3:
		return 70;
	case USERI4:
		return 70;
	case USERI5:
		return 70;
	case USERR1:
		return 40;
	case USERR2:
		return 40;
	case USERR3:
		return 40;
	case USERR4:
		return 40;
	case USERR5:
		return 40;
	case USRTIMER:
		return 70;
		//    case VERSIONGUID:
		//        return 2;
	case VISRETAIN:
		return 70;
	case WORLDVIEW:
		return 70;
		//    case XCLIPFRAME:
		//        return 280;
		//    case XEDIT:
		//        return 290;
	default:
		return -1;
	}
}
Canvas::Canvas()
{
}


Canvas::~Canvas()
{
}

bool Canvas::asFile(const char*fileName)
{
	dxf = new DL_Dxf();
	DL_Codes::version exportVersion = DL_Codes::AC1015;
	dw = dxf->out(fileName, exportVersion);
	if (!dw)
	{
		delete dxf;
		return false;
	}
	SectionHeader();
	TableSectionStart();
	LineTypeTable();
	LayerTypeTable();
	TextStyleTable();
	OtherTable();
	TableSectionEnd();
	BlockSection();
	EntitySection();
	End();
	return true;
}

void Canvas::SectionHeader()
{
	dxf->writeHeader(*dw);
	// int variable:
	dw->dxfString(9, "$INSUNITS");
	dw->dxfInt(70, 4);
	// real (double, float) variable:
	dw->dxfString(9, "$DIMEXE");
	dw->dxfReal(40, 1.25);
 

	// string variable:
	dw->dxfString(9, "$TEXTSTYLE");
	dw->dxfString(7, "Standard");
	 
	dw->dxfString(9, "$DIMBLK");
	dw->dxfString(1, "ArchTick");
	dw->dxfString(9, "$DIMBLK1");
	dw->dxfString(1, "ArchTick");
	dw->dxfString(9, "$DIMBLK2");
	dw->dxfString(1, "ArchTick");
	dw->dxfString(9, "$DIMASZ");
	dw->dxfReal(40, 50);


	//$DIMADEC  70 0int
	/* 
	dw->dxfString(9, "$DIMADEC");
	dw->dxfInt(70, 0);
	//$DIMASZ  40 2.5double
	dw->dxfString(9, "$DIMASZ");
	dw->dxfReal(40, 2.5);
	//$DIMAUNIT  70 0int
	dw->dxfString(9, "$DIMAUNIT");
	dw->dxfInt(70, 0);
	//$DIMAZIN  70 2int
	dw->dxfString(9, "$DIMAZIN");
	dw->dxfInt(70, 2);
	//	$DIMDEC  70 4int
	dw->dxfString(9, "$DIMDEC");
	dw->dxfInt(70, 4);
	//	$DIMDSEP  70 46int
	dw->dxfString(9, "$DIMDSEP");
	dw->dxfInt(70, 46);
	//	$DIMEXE  40 1.25double
	dw->dxfString(9, "$DIMEXE");
	dw->dxfReal(40, 1.25);
	//	$DIMEXO  40 0.625double
	dw->dxfString(9, "$DIMEXO");
	dw->dxfReal(40, 0.625);
	//	$DIMGAP  40 0.625double
	dw->dxfString(9, "$DIMGAP");
	dw->dxfReal(40, 0.625);
	//	$DIMLUNIT  70 2int
	dw->dxfString(9, "$DIMLUNIT");
	dw->dxfInt(70, 2);
	//$DIMSCALE  40 1double
	dw->dxfString(9, "$DIMSCALE");
	dw->dxfReal(40, 1.0);
	//	$DIMTSZ  40 0double
	dw->dxfString(9, "$DIMTSZ");
	dw->dxfReal(40, 0.0);
	//$DIMTXT  40 2.5double
	dw->dxfString(9, "$DIMTSZ");
	dw->dxfReal(40, 2.5);
	//	$DIMZIN  70 8int
	dw->dxfString(9, "$DIMTSZ");
	dw->dxfInt(70, 8);
	//	$DWGCODEPAGE  3 ANSI_1252string
	dw->dxfString(9, "$DWGCODEPAGE");
	dw->dxfString(70,"ANSI_1252");
	//	$INSUNITS  70 4int
	dw->dxfString(9, "$INSUNITS");
	dw->dxfInt(70, 4);
	//$LTSCALE  40 1double
	dw->dxfString(9, "$LTSCALE");
	dw->dxfReal(40, 1);
	//	$MAXACTVP  70 64int
	dw->dxfString(9, "$MAXACTVP");
	dw->dxfInt(70, 64);
	//	$MEASUREMENT  70 1int
	dw->dxfString(9, "$MEASUREMENT");
	dw->dxfInt(70, 1);
			//	$PDMODE  70 0int
	dw->dxfString(9, "$PDMODE");
	dw->dxfInt(70, 0);
			//	$PDSIZE  40 0double
	dw->dxfString(9, "$PDSIZE");
	dw->dxfReal(70, 0.0);
	
	dw->dxfString(9, "$DIMADEC");
	dw->dxfInt(70, 0);

	// vector variable:
	dw->dxfString(9, "$LIMMIN");
	dw->dxfReal(10, 0.0);
	dw->dxfReal(20, 0.0);
	*/
	dw->sectionEnd();
}
 
void Canvas::SectionClasses()
{
	dw->sectionClasses();
	//add MLeaderStyleClass
	dw->dxfString(0, "CLASS");
	dw->dxfString(1, "MLEADERSTYLE");
	dw->dxfString(2, "AcDbMLeaderStyle");
	dw->dxfString(3, "ACDB_MLEADERSTYLE_CLASS");
	dw->dxfInt(90, 4095);
	dw->dxfInt(280, 0);
	dw->dxfInt(281, 0);
	//add Dictionary class
	dw->dxfString(0, "CLASS");
	dw->dxfString(1, "ACDBDICTIONARYWDFLT");
	dw->dxfString(2, "AcDbDictionaryWithDefault");
	dw->dxfString(3, "ObjectDBX Classes");
	dw->dxfInt(90, 0);
	dw->dxfInt(280, 0);
	dw->dxfInt(281, 0);
	 
	dw->sectionEnd();
}

void Canvas::TableSectionStart()
{
	dw->sectionTables();
}

void Canvas::LineTypeTable()
{
	dxf->writeVPort(*dw);
	dw->tableLineTypes(25);
	dxf->writeLineType(*dw, DL_LineTypeData("BYBLOCK", 0));
	dxf->writeLineType(*dw, DL_LineTypeData("BYLAYER", 0));
	dxf->writeLineType(*dw,
		DL_LineTypeData("CONTINUOUS", 0));
	dxf->writeLineType(*dw,
		DL_LineTypeData("ACAD_ISO02W100", 0));
	dxf->writeLineType(*dw,
		DL_LineTypeData("ACAD_ISO03W100", 0));
	dxf->writeLineType(*dw,
		DL_LineTypeData("ACAD_ISO04W100", 0));
	dxf->writeLineType(*dw,
		DL_LineTypeData("ACAD_ISO05W100", 0));
	dxf->writeLineType(*dw, DL_LineTypeData("BORDER", 0));
	dxf->writeLineType(*dw, DL_LineTypeData("BORDER2", 0));
	dxf->writeLineType(*dw, DL_LineTypeData("BORDERX2", 0));
	dxf->writeLineType(*dw, DL_LineTypeData("CENTER", 0));
	dxf->writeLineType(*dw, DL_LineTypeData("CENTER2", 0));
	dxf->writeLineType(*dw, DL_LineTypeData("CENTERX2", 0));
	dxf->writeLineType(*dw, DL_LineTypeData("DASHDOT", 0));
	dxf->writeLineType(*dw, DL_LineTypeData("DASHDOT2", 0));
	dxf->writeLineType(*dw,
		DL_LineTypeData("DASHDOTX2", 0));
	dxf->writeLineType(*dw, DL_LineTypeData("DASHED", 0));
	dxf->writeLineType(*dw, DL_LineTypeData("DASHED2", 0));
	dxf->writeLineType(*dw, DL_LineTypeData("DASHEDX2", 0));
	dxf->writeLineType(*dw, DL_LineTypeData("DIVIDE", 0));
	dxf->writeLineType(*dw, DL_LineTypeData("DIVIDE2", 0));
	dxf->writeLineType(*dw,
		DL_LineTypeData("DIVIDEX2", 0));
	dxf->writeLineType(*dw, DL_LineTypeData("DOT", 0));
	dxf->writeLineType(*dw, DL_LineTypeData("DOT2", 0));
	dxf->writeLineType(*dw, DL_LineTypeData("DOTX2", 0));
	dw->tableEnd();
}

void Canvas::LayerTypeTable()
{
	
	if (!layerArray.size())
		return;
	dw->tableLayers((int)layerArray.size());
	for (auto layer : layerArray)
	{
		Layer* mLayer = dynamic_cast<Layer*>(layer);
		dxf->writeLayer(*dw,
			mLayer->data,
			mLayer->attribute
		);
	}

	dw->tableEnd();
}

void Canvas::TextStyleTable()
{
	dxf->tableStyle(*dw, (int)textStyleArray.size() + 1);
	DL_StyleData style("Standard",
		0,    // flags
		0.0,  // fixed height (not fixed)
		1,  // width factor
		0.0,  // oblique angle
		0,    // text generation flags
		2.5,  // last height used
		utf82gbk("仿宋").c_str(),
		""    // big font file
	);
	style.bold = false;
	style.italic = false;
	dxf->writeStyle(*dw, style);

	for (auto pStyle : textStyleArray)
	{
		dxf->writeStyle(*dw, ((TextStyle*)pStyle)->data);
	}
	dw->tableEnd();
}

void Canvas::OtherTable()
{
	dxf->writeView(*dw);
	dxf->writeUcs(*dw);
	dxf->writeMLeader(*dw);
	dxf->writeDimStyle(*dw, 50, 0.625, 0.625, 0.625, 120);// 120字体大小
	dxf->writeBlockRecord(*dw);

	for (auto block : blockArray)
	{
		Block* ptr = dynamic_cast<Block*>(block);
		dxf->writeBlockRecord(*dw, ptr->blockName);
	}
	dw->tableEnd();
}

void Canvas ::TableSectionEnd()
{
	dw->sectionEnd();
}

void Canvas::BlockSection()
{
	dw->sectionBlocks();

	dxf->writeBlock(*dw,
		DL_BlockData("*Model_Space", 0, 0.0, 0.0, 0.0));
	dxf->writeEndBlock(*dw, "*Model_Space");

	dxf->writeBlock(*dw,
		DL_BlockData("*Paper_Space", 0, 0.0, 0.0, 0.0));
	dxf->writeEndBlock(*dw, "*Paper_Space");

	dxf->writeBlock(*dw,
		DL_BlockData("*Paper_Space0", 0, 0.0, 0.0, 0.0));
	dxf->writeEndBlock(*dw, "*Paper_Space0");

	for (auto iblock : blockArray)
	{
		Block* block = dynamic_cast<Block*>(iblock);
		block->asDXF(dw, dxf);
	}
	dw->sectionEnd();

}

void Canvas::EntitySection()
{
	dw->sectionEntities();
	for (auto ref : entitysArray)
	{
		auto Ientity = dynamic_cast<IEntity*>(ref);
		Ientity->asDXF(dw, dxf);
	}
	dw->sectionEnd();
}

void Canvas::End()
{
	dxf->writeObjects(*dw);
	//param fontSize on base line
	dxf->writeObjectMLeaderStyle(*dw, 40.0);
	dxf->writeObjectsEnd(*dw);
	dw->dxfEOF();
	dw->close();

 
	delete dw;
	delete dxf;
}

 void Canvas::addLineType(ILineType*lineType)
{
	if (!lineType)
		return;
	auto ptr = (LineType*)lineType;
	 
}

 std::string Canvas::utf82gbk(const std::string& utf8)
 {
	 int len = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);
	 wchar_t* wszGBK = new wchar_t[len + 1];
	 memset(wszGBK, 0, len * 2 + 2);
	 MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, wszGBK, len);
	 len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	 char* szGBK = new char[len + 1];
	 memset(szGBK, 0, len + 1);
	 WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	 std::string strTemp(szGBK);
	 if (wszGBK) delete[] wszGBK;
	 if (szGBK) delete[] szGBK;
	 return strTemp;
 }

 void Canvas::addLayer(AttributeEntity*entity)
{
	if (!entity)
		return;
	layerArray.push_back(entity);
}

 void Canvas::addBlock(IBlock*block)
{
	blockArray.push_back(block);
}


 void Canvas::addTextStyle(ITextStyle*style)
 {
	 textStyleArray.push_back(style);
}

 void Canvas::clear()
 {
	 Block::clear();
	 for (auto textStyle : textStyleArray)
		 textStyle->release();
	 textStyleArray.clear();
	 for (auto layer : layerArray)
		 layer->release();
	 layerArray.clear();
	 blockArray.clear();
 }