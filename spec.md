# Specification
## UI
- The user can select files of relevant types to be added to a list of files to be either checked or converted. 
- When user
- Colors: Not started - blue, In progress - yellow, successfully finished - green, failed - red, grey - N/A (for  convert/std when checking, and checking when converting non-swc)
- Blue buttons may be pressed (relevant for checking only), but not other colors
- Zip at end of list will zip any green files.
## Backend/API
- Files will be stored up to 24 h after last interaction, they will then be deleted.
- Provides methods for:
    - Init session
    - Single convert of one upload
    - Single check of one upload
    - Batch upload
    - Batch convert
    - Batch check
    - Delete session