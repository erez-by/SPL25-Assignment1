#include "MixingEngineService.h"
#include <iostream>
#include <memory>


MixingEngineService::MixingEngineService()
    : active_deck(0),auto_sync(false),bpm_tolerance(0)
{
    decks[0] = nullptr;
    decks[1] = nullptr;
    active_deck = 0;
    std::cout << "[MixingEngineService] Initialized with 2 empty decks." << std::endl;

}


MixingEngineService::~MixingEngineService() {
    std::cout << "[MixingEngineService] Cleaning up decks...." << std::endl;
    //Delete both tracks from decks array.
    if(decks[0] != nullptr) {
    delete decks[0];
    decks[0] = nullptr;
    }
    if(decks[1] != nullptr) {
    delete decks[1];
    decks[1] = nullptr;
}

}


/**
 * TODO: Implement loadTrackToDeck method
 * @param track: Reference to the track to be loaded
 * @return: Index of the deck where track was loaded, or -1 on failure
 */
int MixingEngineService::loadTrackToDeck(const AudioTrack& track) {

    std::cout << "\n=== Loading Track to Deck ===\n";
    
    PointerWrapper<AudioTrack> clonedWrapper = track.clone();
    //returning an error if the loading process failed
    if (!clonedWrapper) {
    std::cout << "[ERROR] Track: \"" << track.get_title() << "\" failed to clone\n";
    return -1;
    }
    //Finding the non-active deck
    int target_deck = 1 - active_deck;
    std::cout << "[Deck Switch] Target deck: " << target_deck << "\n";
    //If the deck is occupied, deleting the previous deck
    if (decks[target_deck]) {
    std::cout << "[Unload] Deck " << target_deck << " unloading previous track: " 
              << decks[target_deck]->get_title() << "\n";
    delete decks[target_deck];
    decks[target_deck] = nullptr;
    }
    //loading the track
    clonedWrapper->load();
    clonedWrapper->analyze_beatgrid();
    //Managing the BPM
    if (auto_sync && decks[active_deck]) {
        if (!can_mix_tracks(clonedWrapper)) {
            sync_bpm(clonedWrapper);
        }
    }
    //transfering the responsebility of the treck from PointeWrapper to this class
    decks[target_deck] = clonedWrapper.release();
    std::cout << "[Load Complete] '" << decks[target_deck]->get_title() 
          << "' is now loaded on deck " << target_deck << "\n";
    //unloading the previous active treck
    if (decks[active_deck]) {
        std::cout << "[Unload] Unloading previous deck " << active_deck 
                << " (" << decks[active_deck]->get_title() << ")\n";
        delete decks[active_deck];
        decks[active_deck] = nullptr;
    }
    //switch active deck
    active_deck = target_deck;
    std::cout << "[Active Deck] Switched to deck " << active_deck << "\n";

    return active_deck; //Return target deck index for statistics tracking
}



/**
 * @brief Display current deck status
 */
void MixingEngineService::displayDeckStatus() const {
    std::cout << "\n=== Deck Status ===\n";
    for (size_t i = 0; i < 2; ++i) {
        if (decks[i])
            std::cout << "Deck " << i << ": " << decks[i]->get_title() << "\n";
        else
            std::cout << "Deck " << i << ": [EMPTY]\n";
    }
    std::cout << "Active Deck: " << active_deck << "\n";
    std::cout << "===================\n";
}

/**

 * Check if two tracks can be mixed based on BPM difference.
 * 
 * @param track: Track to check for mixing compatibility
 * @return: true if BPM difference <= tolerance, false otherwise
 */
bool MixingEngineService::can_mix_tracks(const PointerWrapper<AudioTrack>& track) const {
    if (!decks[active_deck]) { return false; }
    if (!track) { return false; }
    //Getting the BPM of both tracks
    int bpm_old = decks[active_deck]->get_bpm();
    int bpm_new = track->get_bpm();
    //calculating the absolute difference between the bpms
    int bpm_difference = std::abs(bpm_old - bpm_new); 

    return bpm_difference <= bpm_tolerance;
}

/**
 * TODO: Implement sync_bpm method
 * @param track: Track to synchronize with active deck
 */
void MixingEngineService::sync_bpm(const PointerWrapper<AudioTrack>& track) const {
    if (decks[active_deck] && track) {
        //Getting the BPM of both tracks
        int bpm_old = decks[active_deck]->get_bpm();
        int bpm_new = track->get_bpm();
        //Calctlating average
        int average_bpm = (bpm_old + bpm_new) / 2;
        //updating the input track bpm
        track->set_bpm(average_bpm);

        std::cout << "[Sync BPM] Syncing BPM from " 
          << bpm_new << " to " <<  average_bpm << "\n";

    }
}
