#include <XPLMPlugin.h>
#include <XPLMUtilities.h>
#include <XPLMDataAccess.h>
#include <XPLMProcessing.h>

#include <curl/curl.h>

#include <cstring>
#include <stdio.h>


XPLMDataRef dataRefLat;
XPLMDataRef dataRefLon;
XPLMDataRef dataRefAlt;
XPLMDataRef dataRefPayload;

void doCurlThings();
void dumpPlayerLatLong();
float doThings (
	float                inElapsedSinceLastCall,
	float                inElapsedTimeSinceLastFlightLoop,
	int                  inCounter,
	void *               inRefcon);

PLUGIN_API int XPluginStart(char * outName, char * outSig, char * outDesc) {
    // Plugin details
	strcpy(outName, "PilotsMP XP11 Plugin");
	strcpy(outSig, "com.businessinsight.pilotsmp");
	strcpy(outDesc, "Tomorrow's Virtual Economy...Today.");

	XPLMEnableFeature("XPLM_USE_NATIVE_PATHS", 1);

	XPLMDebugString("PilotsMP XP11 Plugin Started\n");

	XPLMRegisterFlightLoopCallback(doThings, 1, 0);

	dataRefLat = XPLMFindDataRef("sim/flightmodel/position/latitude");
	dataRefLon = XPLMFindDataRef("sim/flightmodel/position/longitude");
	dataRefAlt = XPLMFindDataRef("sim/flightmodel/position/elevation");
	dataRefPayload = XPLMFindDataRef("sim/flightmodel/weight/m_fixed");
	
	dumpPlayerLatLong();

	doCurlThings();

	return 1;
}

PLUGIN_API void	XPluginStop(void) {
	XPLMDebugString("PilotsMP XP11 Plugin Stopped\n");
	dumpPlayerLatLong();
}

PLUGIN_API void XPluginDisable(void) {
	XPLMDebugString("PilotsMP XP11 Plugin Disabled\n");
	dumpPlayerLatLong();
}

PLUGIN_API int XPluginEnable(void) {
	XPLMDebugString("PilotsMP XP11 Plugin Enabled\n");
	dumpPlayerLatLong();
	return 1;
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID, intptr_t inMessage, void * inParam) {
}

void dumpPlayerLatLong() {
	double playerLat = XPLMGetDatad(dataRefLat);
	double playerLon = XPLMGetDatad(dataRefLon);
	double playerAlt = XPLMGetDatad(dataRefAlt);

	float playerPayload = XPLMGetDataf(dataRefPayload);
	playerPayload += 5.0f;
	XPLMSetDataf(dataRefPayload, playerPayload);

	char buffer[128];
	sprintf(buffer, "player lat: %f lon: %f alt: %f payload: %f\n", playerLat, playerLon, playerAlt, playerPayload);
	XPLMDebugString(buffer);
}

float doThings(float inElapsedSinceLastCall,
	float inElapsedTimeSinceLastFlightLoop,
	int inCounter,
	void *inRefcon)
{
	dumpPlayerLatLong();
	return 1.0f;
}

void doCurlThings() {
	CURL *curl;
	CURLcode res;

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "http://google.com");
		/* example.com is redirected, so we tell libcurl to follow redirection */
		curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		/* Check for errors */
		if (res != CURLE_OK) {
			char buffer[2048];
			sprintf(buffer, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			XPLMDebugString(buffer);

			//fprintf(stderr, "curl_easy_perform() failed: %s\n",
			//	curl_easy_strerror(res));
		}
		else {
			XPLMDebugString("PilotsMP - LibCurl completed request without error");
		}
		
		/* always cleanup */
		curl_easy_cleanup(curl);
	}
}