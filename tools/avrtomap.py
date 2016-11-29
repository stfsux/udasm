# pdftotext required.
import os
import sys
import re
import getopt

EXTRACT_NONE = 0
EXTRACT_IDATA = 1
EXTRACT_INT = 2
EXTRACT_IDATA_BITS = 4

def atmel_datasheets_iregs_model1 (line):
  match_obj = re.match ('\s*(0x[0-9|A-F|a-f]{2})\s+([A-Z|a-z|0-9]+)\s*.*', line)
  if match_obj is None:
    return None
  offset = match_obj.group(1)
  regname = match_obj.group(2)
  if regname == "Reserved":
    return None
  return [regname, offset]

def atmel_datasheets_ints_model1 (line):
  match_obj = re.match ('\s*[0-9]+\s+(0x[0-9|A-F|a-f]+)\s+([A-Z|a-z|0-9|_]+)\s*.*', line)
  if match_obj is None:
    return None
  offset = match_obj.group(1)
  regname = match_obj.group(2)
  if regname == "Reserved":
    return None
  return [regname, offset]

def atmel_datasheets_read_iregs (filename, fp, lp):
  list_internal_regs = list ()
  input_pdf_filename = filename
  input_txt_filename = str()
  input_pdf_fp = fp
  input_pdf_lp = lp
  input_txt_filename = input_pdf_filename.replace ('.pdf', '.txt')

  os.system ('pdftotext -f %s -l %s -layout %s' %
      (input_pdf_fp, input_pdf_lp, input_pdf_filename))

  input_txt_fd = open (input_txt_filename, 'r')
  input_txt_lines = input_txt_fd.read().splitlines()
  for line in input_txt_lines:
    if line.find ('Note') != -1:
      break
    a = atmel_datasheets_iregs_model1 (line)
    if a is None:
      continue
    list_internal_regs.append (a)
  input_txt_fd.close ()
  return list_internal_regs

def atmel_datasheets_read_ints (filename, fp, lp):
  list_ints = list ()
  input_pdf_filename = filename
  input_txt_filename = str()
  input_pdf_fp = fp
  input_pdf_lp = lp
  input_txt_filename = input_pdf_filename.replace ('.pdf', '.txt')

  os.system ('pdftotext -f %s -l %s -layout %s' %
      (input_pdf_fp, input_pdf_lp, input_pdf_filename))

  input_txt_fd = open (input_txt_filename, 'r')
  input_txt_lines = input_txt_fd.read().splitlines()
  for line in input_txt_lines:
    if line.find ('Note') != -1:
      break
    a = atmel_datasheets_ints_model1 (line)
    if a is None:
      continue
    list_ints.append (a)
  input_txt_fd.close ()
  return list_ints 

def main():
  mode = EXTRACT_NONE
  input_pdf_filename = str()
  output_cfg_filename = str()
  ireg_fp = str()
  ireg_lp = str()
  int_fp = str()
  int_lp = str()
  list_internal_regs = list ()
  list_ints = list ()
  longopt = [
  'help',
  'idata',
  'idata-bit',
  'int',
  'output'
  ]
  shortopt = 'hr:b:i:o:'

  try:
    opts, args = getopt.getopt (sys.argv[1:], shortopt, longopt)
  except getopt.GetoptError as err:
    usage ()
    sys.exit (0)
  for o, a in opts:
    if o in ('-h', '--help'):
      usage ()
      sys.exit (0)
    elif o in ('-r', '--idata'):
      mode = mode | EXTRACT_IDATA
      m = re.match ('([0-9]+),([0-9]+)', a)
      if m is None:
        usage ()
        sys.exit (0)
      ireg_fp = m.group(1)
      ireg_lp = m.group(2)
    elif o in ('-i', '--int'):
      mode = mode | EXTRACT_INT
      m = re.match ('([0-9]+),([0-9]+)', a)
      if m is None:
        usage ()
        sys.exit (0)
      int_fp = m.group(1)
      int_lp = m.group(2)
    elif o in ('-o', '--output'):
      output_cfg_filename = a
    else:
      usage ()
      sys.exit (0)

  if len(args) != 1:
    usage ()
    sys.exit (0)

  if output_cfg_filename == "":
    usage ()
    sys.exit (0)

  input_pdf_filename = args[0]

  if (mode & EXTRACT_INT):
    list_ints = atmel_datasheets_read_ints (input_pdf_filename, int_fp, int_lp)
    udasm_ints_to_cfg (output_cfg_filename, list_ints)
  if (mode & EXTRACT_IDATA):
    list_internal_regs = atmel_datasheets_read_iregs (input_pdf_filename, ireg_fp, ireg_lp)
    udasm_iregs_to_cfg (output_cfg_filename, list_internal_regs)

  sys.exit (0)

def udasm_iregs_to_cfg (output_filename, internal_regs):
  n = 0
  output_fd = open (output_filename, 'a+')
  output_fd.write ('IDATA:\n{\n  registers = \n  (')
  for reg in internal_regs:
    if n == 0:
      output_fd.write ('\n    ( \"%s\", %s )' % (reg[0], reg[1]))
    else:
      output_fd.write (',\n    ( \"%s\", %s )' % (reg[0], reg[1]))
    n = n + 1
  output_fd.write ('\n  );\n}\n\n')
  output_fd.close ()

def udasm_ints_to_cfg (output_filename, ints):
  n = 0
  size = 0
  output_fd = open (output_filename, 'w')
  output_fd.write ('/* file generated with avrtomap.py */\n\n')
  output_fd.write ('CODE:\n{\n  interrupts = \n  (')
  for interrupt in ints:
    if ints.index(interrupt) != len(ints)-1:
      nitem = ints[ints.index(interrupt)+1]
      size = (int(nitem[1], 16) - int(interrupt[1], 16))*2
    if n == 0:
      output_fd.write ('\n    ( \"%s\", %s, 0x%02X, "code" )' %
        (interrupt[0], interrupt[1], size))
    else:
      output_fd.write (',\n    ( \"%s\", %s, 0x%02X, "code" )' %
        (interrupt[0], interrupt[1], size))
    n = n + 1
  output_fd.write ('\n  );\n}\n\n')
  output_fd.close ()

def usage ():
  sys.stdout.write ('%s: [OPTIONS] ... [FILE]\n'
      '-h,--help            Print this shit\n'
      '-r,--idata p1,p2     Extract list of internal registers\n'
      '                     from page p1 to p2.\n'
      '-b,--idata-bit p1,p2 Extract list of bit-addressable\n'
      '                     of internal registers from page\n'
      '                     p1 to p2.\n'
      '-i,--int p1,p2       Extract list of interrupts from\n'
      '                     page p1 to p2.\n' % sys.argv[0])

main ()
