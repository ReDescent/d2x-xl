
#include <string.h>
#include <stdio.h>
#include "trackir.h"

// ------------------------------------------------------------------------------------------
// this is what comes from the trackir software

# pragma pack(push, packing)
# pragma pack(1)

#define	TIR_DEVELOPER_ID	16001

#define	TIR_QUERY_VERSION	1040

#define	TIR_STATUS_REMOTE_ACTIVE	0
#define	TIR_STATUS_REMOTE_DISABLED	1

#define	TIR_VERSIONMAJOR	1
#define	TIR_VERSIONMINOR	2

// DATA FIELDS
#define	NPControl		8	// indicates a control data field
#define	TIR_ROLL			1	// +/- 16383 (representing +/- 180) [data = input - 16383]
#define	TIR_PITCH		2	// +/- 16383 (representing +/- 180) [data = input - 16383]
#define	TIR_YAW			4	// +/- 16383 (representing +/- 180) [data = input - 16383]
// x, y, z - remaining 6dof coordinates
#define	TIR_X				16	// +/- 16383 [data = input - 16383]
#define	TIR_Y				32	// +/- 16383 [data = input - 16383]
#define	TIR_Z				64	// +/- 16383 [data = input - 16383]
// raw object position from imager
#define	TIR_RAW_X		128	// 0..25600 (actual value is multiplied x 100 to pass two decimal places of precision)  [data = input / 100]
#define	TIR_RAW_Y		256  // 0..25600 (actual value is multiplied x 100 to pass two decimal places of precision)  [data = input / 100]
#define	TIR_RAW_Z		512  // 0..25600 (actual value is multiplied x 100 to pass two decimal places of precision)  [data = input / 100]
// x, y, z deltas from raw imager position 
#define	TIR_DELTA_X		1024 // +/- 2560 (actual value is multiplied x 10 to pass two decimal places of precision)  [data = (input / 10) - 256]
#define	TIR_DELTA_Y		2048 // +/- 2560 (actual value is multiplied x 10 to pass two decimal places of precision)  [data = (input / 10) - 256]
#define	TIR_DELTA_Z		4096 // +/- 2560 (actual value is multiplied x 10 to pass two decimal places of precision)  [data = (input / 10) - 256]
// raw object position from imager
#define	TIR_SMOOTH_X	8192	  // 0..32766 (actual value is multiplied x 10 to pass one decimal place of precision) [data = input / 10]
#define	TIR_SMOOTH_Y	16384  // 0..32766 (actual value is multiplied x 10 to pass one decimal place of precision) [data = input / 10]
#define	TIR_SMOOTH_Z	32768  // 0..32766 (actual value is multiplied x 10 to pass one decimal place of precision) [data = input / 10]

typedef enum tTIRStatus {
	TIR_OK = 0,
   TIR_ERR_DEVICE_NOT_PRESENT,
	TIR_ERR_UNSUPPORTED_OS,
	TIR_ERR_INVALID_ARG,
	TIR_ERR_DLL_NOT_FOUND,
	TIR_ERR_NO_DATA,
	TIR_ERR_INTERNAL_DATA
	} tTIRStatus;

typedef struct tTIRSignature {
	char szDllSig [200];
	char szAppSig [200];
	} tTIRSignature;

typedef struct tTIRInterface {
	unsigned short	nStatus;		// how did the request perform?
	unsigned short	nFrameSig;	// unique id for each frame of the trackir software
	unsigned long	nDataFlags;	// what data fields are set?
	fVector3D		fvRot;		// rotation angles
	fVector3D		fvTrans;		// axis offsets
	fVector3D		fvRaw;		// raw position data
	fVector3D		fvDelta;		// difference to previous frame
	fVector3D		fvSmooth;	// fractional raw position data
	} tTIRInterface;

typedef tTIRStatus (__stdcall *tpfnTIRNotifyCallback) (unsigned short, unsigned short);

typedef tTIRStatus (__stdcall *tpfnTIRRegisterWindowHandle) (HWND);
typedef tTIRStatus (__stdcall *tpfnTIRUnregisterWindowHandle) (void);
typedef tTIRStatus (__stdcall *tpfnTIRRegisterProgramProfileID) (unsigned short);
typedef tTIRStatus (__stdcall *tpfnTIRQueryVersion) (unsigned short *);
typedef tTIRStatus (__stdcall *tpfnTIRRequestData) (unsigned short);
typedef tTIRStatus (__stdcall *tpfnTIRGetData) (tTIRInterface *);
typedef tTIRStatus (__stdcall *tpfnTIRGetSignature) (tTIRSignature *);
typedef tTIRStatus (__stdcall *tpfnTIRRegisterNotify) (tpfnTIRNotifyCallback);
typedef tTIRStatus (__stdcall *tpfnTIRUnregisterNotify) (void);
typedef tTIRStatus (__stdcall *tpfnTIRStartCursor) (void);
typedef tTIRStatus (__stdcall *tpfnTIRStopCursor) (void);
typedef tTIRStatus (__stdcall *tpfnTIRReCenter) (void);
typedef tTIRStatus (__stdcall *tpfnTIRStartDataTransmission) (void);
typedef tTIRStatus (__stdcall *tpfnTIRStopDataTransmission) (void);

#pragma pack(pop, __TRACKIR_H) // Ensure previous pack value is restored

// ------------------------------------------------------------------------------------------
// data and code

static HINSTANCE	hTIRDll = 0;

static tpfnTIRRegisterWindowHandle RegisterWindowHandle = NULL;
static tpfnTIRUnregisterWindowHandle UnregisterWindowHandle = NULL;
static tpfnTIRRegisterProgramProfileID RegisterProgramProfileID = NULL;
static tpfnTIRQueryVersion QueryVersion = NULL;
static tpfnTIRRequestData RequestData = NULL;
static tpfnTIRGetSignature GetSignature = NULL;
static tpfnTIRGetData GetData = NULL;
static tpfnTIRRegisterNotify RegisterNotify = NULL;
static tpfnTIRUnregisterNotify UnregisterNotify = NULL;
static tpfnTIRStartCursor StartCursor = NULL;
static tpfnTIRStopCursor StopCursor = NULL;
static tpfnTIRReCenter ReCenter = NULL;
static tpfnTIRStartDataTransmission StartDataTransmission = NULL;
static tpfnTIRStopDataTransmission StopDataTransmission = NULL;

static tTIRSignature refSig = {
	"precise head tracking\n put your head into the game\n now go look around\n\n Copyright EyeControl Technologies",
	"hardware camera\n software processing data\n track user movement\n\n Copyright EyeControl Technologies"
	};

static tTIRInterface	tirData;

// ------------------------------------------------------------------------------------------

char *TIRDllPath (char *pszPath)
{
	HKEY		pKey = NULL;
	LPTSTR	pszValue;
	DWORD		dwSize;

if (!pszPath)
	return pszPath;
*pszPath = '\0';
if (RegOpenKeyEx (HKEY_CURRENT_USER, "Software\\NaturalPoint\\NATURALPOINT\\NPClient Location", 0, KEY_READ, &pKey) != ERROR_SUCCESS)
	return NULL;
if (RegQueryValueEx (pKey, "Path", NULL, NULL, NULL, &dwSize) != ERROR_SUCCESS)
	return NULL;
if (!(pszValue = (LPTSTR) malloc (dwSize)))
	return NULL;
if (RegQueryValueEx (pKey, "Path", NULL, NULL, (LPBYTE) pszValue, &dwSize) != ERROR_SUCCESS) {
	RegCloseKey (pKey);
	return NULL;
	}
strcpy (pszPath, pszValue);
free (pszValue);
return pszPath;
}

// ------------------------------------------------------------------------------------------

static int TIRLoaded (void)
{
return (size_t) hTIRDll >= HINSTANCE_ERROR;
}

// ------------------------------------------------------------------------------------------

static int TIRFree (void)
{
if (hTIRDll)
   FreeLibrary (hTIRDll);
hTIRDll = 0;
return 0;
}

// ------------------------------------------------------------------------------------------

#define	LOAD_TIR_FUNC(_t,_f) \
			if (!((_f) = (_t) GetProcAddress (hTIRDll, "NP_" #_f))) \
				return TIRFree ();

static int TIRLoad (void)
{
	char				szPath [MAX_PATH], szFile [2 * MAX_PATH];
	tTIRSignature	sig;

if (TIRLoaded ())
	return 1;
TIRDllPath (szPath);
//strcat (szPath, "NPClient.dll");
sprintf (szFile, "%sNPClient.dll", szPath);
hTIRDll = LoadLibrary (szFile);
if ((size_t) hTIRDll < HINSTANCE_ERROR)
	return 0;
LOAD_TIR_FUNC (tpfnTIRGetSignature, GetSignature)
if (GetSignature (&sig) != TIR_OK)
	return TIRFree ();
if (strcmp (sig.szDllSig, refSig.szDllSig) || strcmp (sig.szAppSig, refSig.szAppSig))
	return TIRFree ();
LOAD_TIR_FUNC (tpfnTIRRegisterWindowHandle, RegisterWindowHandle)
LOAD_TIR_FUNC (tpfnTIRUnregisterWindowHandle, UnregisterWindowHandle)
LOAD_TIR_FUNC (tpfnTIRRegisterProgramProfileID, RegisterProgramProfileID)
LOAD_TIR_FUNC (tpfnTIRQueryVersion, QueryVersion)
LOAD_TIR_FUNC (tpfnTIRRequestData, RequestData)
LOAD_TIR_FUNC (tpfnTIRGetData, GetData)
LOAD_TIR_FUNC (tpfnTIRStartCursor, StartCursor)
LOAD_TIR_FUNC (tpfnTIRStopCursor, StopCursor)
LOAD_TIR_FUNC (tpfnTIRReCenter, ReCenter)
LOAD_TIR_FUNC (tpfnTIRStartDataTransmission, StartDataTransmission)
LOAD_TIR_FUNC (tpfnTIRStopDataTransmission, StopDataTransmission)
return 1;
}

// ------------------------------------------------------------------------------------------

int WINAPI TIRQuery (tTIRInfo *pti)
{
if (!TIRLoaded ())
	return 0;
if (!GetData || (GetData (&tirData) != TIR_OK))
	return 0;
if (tirData.nStatus != TIR_STATUS_REMOTE_ACTIVE)
	return 0;
pti->fvRot = tirData.fvRot;
pti->fvTrans = tirData.fvTrans;
return 1;
}

// ------------------------------------------------------------------------------------------

int WINAPI TIRStop (void)
{
return TIRLoaded () && StopDataTransmission && (StopDataTransmission () == TIR_OK);
}

// ------------------------------------------------------------------------------------------

int WINAPI TIRStart (void)
{
return TIRLoaded () && StartDataTransmission && (StartDataTransmission () == TIR_OK);
}

// ------------------------------------------------------------------------------------------

int WINAPI TIRCenter (void)
{
return TIRStop () && ReCenter && (ReCenter () == TIR_OK) && TIRStart ();
}

// ------------------------------------------------------------------------------------------

int WINAPI TIRInit (HWND hWnd)
{
	unsigned short	nClientVer;

memset (&tirData, 0, sizeof (tirData));
if (!TIRLoad ())
	return 0;
if (RegisterWindowHandle (hWnd) != TIR_OK)
	return 0;
if (QueryVersion (&nClientVer) != TIR_OK)
	return 0;
// tell the trackir software which data we're interested in
if (RequestData (TIR_PITCH | TIR_YAW | TIR_ROLL | TIR_X | TIR_Y | TIR_Z) != TIR_OK)
	return 0;
RegisterProgramProfileID (TIR_DEVELOPER_ID);
return 1;
}

// ------------------------------------------------------------------------------------------

int WINAPI TIRExit (void)
{
TIRStop ();
if (UnregisterWindowHandle)
	UnregisterWindowHandle ();
return 1;
}

// ------------------------------------------------------------------------------------------

BOOL WINAPI DllMain (HINSTANCE hInstDll, DWORD fdwReason, LPVOID lpvReserved)
{
   static long	loadC = 0;

switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
      ++loadC;
		break;
      
   case DLL_THREAD_ATTACH:
      break;
      
   case DLL_THREAD_DETACH:
      break;
      
   case DLL_PROCESS_DETACH:
      if (loadC && !--loadC)
			TIRExit ();
	}
return 1;
}

// ------------------------------------------------------------------------------------------

// eof
