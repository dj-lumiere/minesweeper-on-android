package com.lumi.minesweeper

class CellData(
    var isMine: Boolean,
    var isRevealed: Boolean,
    var isFlagged: Boolean,
    var adjacentMines: Int
) {
    constructor() : this(false, false, false, 0)

    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (javaClass != other?.javaClass) return false

        other as CellData

        if (isMine != other.isMine) return false
        if (isRevealed != other.isRevealed) return false
        if (isFlagged != other.isFlagged) return false
        if (adjacentMines != other.adjacentMines) return false

        return true
    }

    override fun hashCode(): Int {
        var result = isMine.hashCode()
        result = 31 * result + isRevealed.hashCode()
        result = 31 * result + isFlagged.hashCode()
        result = 31 * result + adjacentMines
        return result
    }

    override fun toString(): String {
        return "CellData(isMine=$isMine, isRevealed=$isRevealed, isFlagged=$isFlagged, adjacentMines=$adjacentMines)"
    }

}