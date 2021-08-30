#pragma once
#include <unordered_map>
#include "../const/appconst.h"

class UIConfig {
	private:

		int infoHolderHeight;
		int cpuUtilizationInfoHolderXOffSet;
		int ipToggleButtonXOffset;
		int infoItemWidth;
		int adJustedBoxItemHeightBottomMargin;
	public:
		UIConfig(int infoHolderHeight, int cpuUtilizationInfoHolderXOffSet, int infoItemWidth,
			int adJustedBoxItemHeightBottomMargin) {
			this->infoHolderHeight = infoHolderHeight;
			this->cpuUtilizationInfoHolderXOffSet = cpuUtilizationInfoHolderXOffSet;
			this->infoItemWidth = infoItemWidth;
			this->adJustedBoxItemHeightBottomMargin = adJustedBoxItemHeightBottomMargin;
		}	

		~UIConfig() {}

		// getters
		int getInfoHolderHeight() {
			return this->infoHolderHeight;
		}

		void setInfoHolderHeight(int infoItemWidth) {
			this->infoItemWidth = infoItemWidth;
		}

		int getCpuUtilizationInfoHolderXOffSet() {
			return cpuUtilizationInfoHolderXOffSet;
		}

		int getIpToggleButtonXOffset() {
			return ipToggleButtonXOffset;
		}

		int getInfoItemWidth() {
			return this->infoItemWidth;
		}
		int getAdJustedBoxItemHeightBottomMargin() {
			return this->adJustedBoxItemHeightBottomMargin;
		}
};

enum class DPI_CONFIG {
		NON_HI_RES_DISPLAY = 0,
		HI_RES_DISPLAY
};

extern DPI_CONFIG dpiUiConfig;

const static std::unordered_map <DPI_CONFIG, UIConfig> dpiConf = {
{ DPI_CONFIG::NON_HI_RES_DISPLAY, UIConfig(1, ITEM_NON_HI_DPI_CPU_UTIL_OFFSET,ITEM_WIDTH_NON_HI_DPI, ADJUSTED_BOX_ITEM_HEIGHT_BOTTOM_MARGIN_NON_HI_DPI) },

{ DPI_CONFIG::HI_RES_DISPLAY, UIConfig(1, ITEM_HI_DPI_CPU_UTIL_OFFSET,ITEM_WIDTH_HI_DPI, ADJUSTED_BOX_ITEM_HEIGHT_BOTTOM_MARGIN_HI_DPI) } };





	 