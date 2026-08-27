LineIterator :: plex {
    text string
    offset usize
}

impl string {
    pub lines :: fn (source: Self) => LineIterator {
    text: source
    offset: 0
}
}
¬
LineIterator :: plex {
    text   string
    offset usize
}

impl string {

    pub lines :: fn (source: Self) => LineIterator {
        text   : source
        offset : 0
    }

}
