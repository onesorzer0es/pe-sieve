#pragma once

#include <windows.h>

#include "module_scanner.h"
#include "scanned_modules.h"

namespace pesieve {

	//!  A report from an IAT scan, generated by IATScanner
	class IATScanReport : public ModuleScanReport
	{
	public:

		static bool saveNotRecovered(IN std::string fileName,
			IN HANDLE hProcess,
			IN const std::map<ULONGLONG, peconv::ExportedFunc> *storedFunc,
			IN peconv::ImpsNotCovered &notCovered,
			IN const ModulesInfo &modulesInfo,
			IN const peconv::ExportsMapper *exportsMap);

		IATScanReport(HANDLE processHandle, HMODULE _module, size_t _moduleSize, std::string _moduleFile)
			: ModuleScanReport(processHandle, _module, _moduleSize, SCAN_SUSPICIOUS)
		{
			moduleFile = _moduleFile;
		}

		const virtual bool toJSON(std::stringstream &outs, size_t level, const pesieve::t_json_level &jdetails)
		{
			size_t hooks = countHooked();
			OUT_PADDED(outs, level, "\"iat_scan\" : ");
			outs << "{\n";
			ModuleScanReport::_toJSON(outs, level + 1);
			outs << ",\n";
			OUT_PADDED(outs, level + 1, "\"hooks\" : ");
			outs << std::dec << hooks;
			if (jdetails >= JSON_DETAILS && hooks) {
				outs << ",\n";
				this->hooksToJSON(outs, level + 1);
			}
			outs << "\n";
			OUT_PADDED(outs, level, "}");
			return true;
		}

		bool generateList(IN const std::string &fileName, IN HANDLE hProcess, IN const ModulesInfo &modulesInfo, IN const peconv::ExportsMapper *exportsMap);

		const bool hooksToJSON(std::stringstream &outs, size_t level);
		size_t countHooked() { return notCovered.count(); }

		std::map<ULONGLONG, peconv::ExportedFunc> storedFunc;
		peconv::ImpsNotCovered notCovered;

	};

	//---

	//!  A scanner for detection of IAT hooking.
	class IATScanner : public ModuleScanner {
	public:

		IATScanner::IATScanner(
			HANDLE hProc,
			ModuleData &moduleData,
			RemoteModuleData &remoteModData,
			const peconv::ExportsMapper &_exportsMap,
			IN const ModulesInfo &_modulesInfo,
			t_iat_scan_mode _hooksFilter
		)
			: ModuleScanner(hProc, moduleData, remoteModData),
			exportsMap(_exportsMap), modulesInfo(_modulesInfo),
			hooksFilter(_hooksFilter)
		{
			initExcludedPaths();
		}

		virtual IATScanReport* scanRemote();

	private:
		void initExcludedPaths();

		bool hasImportTable(RemoteModuleData &remoteModData);
		bool filterResults(peconv::ImpsNotCovered &not_covered, IATScanReport &report);
		void listAllImports(std::map<ULONGLONG, peconv::ExportedFunc> &_storedFunc);

		const peconv::ExportsMapper &exportsMap;
		const ModulesInfo &modulesInfo;

		t_iat_scan_mode hooksFilter;
		
		//excluded paths:
		std::string m_sysWow64Path_str;
		std::string m_system32Path_str;
	};

}; //namespace pesieve

