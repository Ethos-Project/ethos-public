pub struct MorphoLemmaFeature {
    pub surface_form: str,
    pub lemma: str,
    pub pos_tag: str,
    pub inflections: map[str, str],
}

pub struct IrregularMorphologyRecord {
    pub lemma: str,
    pub pos_tag: str,
    pub inflections: map[str, str],
}

pub struct IrregularVerbLexicon {
    pub registry: map[str, IrregularMorphologyRecord],
}

@query
pub fn lookup_surface_form(lexicon: &IrregularVerbLexicon, surface_word: str) -> Option<IrregularMorphologyRecord> {
    // Queries the irregular matrix block for matches.
}
