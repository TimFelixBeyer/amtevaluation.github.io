"""A python wrapper for MUSTER."""
import copy
import shutil
import subprocess
import tempfile
import os
import xml.etree.ElementTree as ET
import zipfile
from pathlib import Path

from music21 import musicxml, note, stream

MUSTER_BIN = Path(__file__).resolve().parent / "evaluate_XML_voicePlus.sh"
EMPTY_SCORES = {'PitchER': None, 'MissRate': None, 'ExtraRate': None, 'OnsetER': None, 'OffsetER': None, 'MeanER': None, 'VoiceER': None, 'NewMeanER': None, 'Pvoi': None, 'Rvoi': None, 'Fvoi': None, 'ScaleErr': None, 'HandER': None}


def postprocess_score(mxl: stream.Score) -> stream.Score:
    """We essentially roll our own "makeNotation" here because music21's is broken
    for non-ideal scores.
    """
    mxl = copy.deepcopy(mxl)
    mxl = mxl.splitAtDurations(recurse=True)[0]
    mxl = mxl.makeTies()

    # The following code is fairly ugly cleanup.
    # It removes empty voices and pads under-full measures with rests.
    for part in mxl.parts:
        measures: list[stream.Measure] = list(part.getElementsByClass("Measure"))
        for m in measures:
            for v in list(m.getElementsByClass("Voice")):
                if not v.notes:
                    m.remove(v)
            # pad non-full measures with rests
            for j, v in enumerate(m.getElementsByClass("Voice")):
                v: stream.Voice
                # Clean up overlaps
                v.id = str(j + 1)
                if m.highestTime < m.barDuration.quarterLength:
                    quarterLength = m.barDuration.quarterLength - v.highestTime
                    rest = note.Rest(quarterLength=quarterLength)
                    v.append(rest.splitAtDurations())

            if not list(m.getElementsByClass("Voice")):
                v = stream.Voice()
                v.id = "1"
                rest = note.Rest(quarterLength=m.barDuration.quarterLength)
                v.append(rest.splitAtDurations())
                m.insert(0, v)
    mxl = mxl.splitAtDurations(recurse=True)[0]
    return mxl


def muster(
    score_est: stream.Score | str | Path,
    score_gt: stream.Score | str | Path,
    clean_scores: bool=True
) -> dict[str, float|None]:
    """
    Computes the MUSTER score between two music scores.

    Parameters
    ----------
        score_est (stream.Score | str | Path): The estimated music score, either as MusicXML path or music21 score.
        score_gt (stream.Score | str | Path): The ground truth music score, either as MusicXML path or music21 score.
        clean_scores (bool, optional): Whether to clean the scores before computing the score. Defaults to True.

    Returns
    -------
        dict[str, float]: A dictionary containing the MUSTER score and its components.
    """
    with tempfile.TemporaryDirectory() as tmpdir:
        try:
            handle_score_file(score_est, tmpdir + "/est.xml", clean_scores)
            handle_score_file(score_gt, tmpdir + "/gt.xml", clean_scores)
        except ValueError as e:
            print(e)
            return EMPTY_SCORES
        subprocess.run(
            [MUSTER_BIN, tmpdir + "/gt", tmpdir + "/est", tmpdir + "/out"],
            stdout=subprocess.DEVNULL,
        )
        try:
            with open(tmpdir + "/out.txt") as f:
                lines = f.readlines()
        except FileNotFoundError as e:
            return EMPTY_SCORES
    try:
        d = parse_line_to_dict(lines[0])
    except ValueError as e:
        print(lines)
        print(e, lines[0])
        return EMPTY_SCORES
    return d


def handle_score_file(score, out_path, clean_scores):
    if isinstance(score, stream.Score):
        if clean_scores:
            score = postprocess_score(score)
        try:
            score.write("musicxml", out_path, makeNotation=False)
        except musicxml.xmlObjects.MusicXMLExportException as e:
            raise ValueError(f"MusicXML export error: {e}") from e
    elif isinstance(score, (str, Path)) and Path(score).suffix in [".musicxml", ".xml", '.mxl']:
        if Path(score).suffix == '.mxl':
            with tempfile.TemporaryDirectory() as temp_dir:
                with zipfile.ZipFile(score, "r") as z:
                    z.extractall(temp_dir)
                for xml_file in Path(temp_dir).rglob("container.xml"):
                    with open(xml_file, "rb") as f:
                        tree = ET.parse(f)
                        root = tree.getroot()
                    score_path = None
                    for rootfile in root.findall(".//rootfile"):
                        score_path = rootfile.attrib.get("full-path")
                        if score_path is not None:
                            break
                    if score_path is None:
                        raise ValueError("Could not read MXL file")
                    score_path = os.path.join(temp_dir, score_path)
                    shutil.copy(score_path, out_path)
        else:
            shutil.copyfile(score, out_path)
    else:
        raise ValueError("Invalid file type or extension. Must be .musicxml or .xml")


def parse_line_to_dict(line):
    keys, values = line.split(":")
    keys_list = keys.strip().split(",")
    values_list = values.split()
    assert len(keys_list) == len(values_list), "Parsing results went wrong!"
    # Zip the keys and values to create the dictionary
    result_dict = dict(zip(keys_list, [float(val) for val in values_list]))
    return result_dict
