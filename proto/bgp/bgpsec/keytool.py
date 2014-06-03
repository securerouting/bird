#!/usr/bin/env python
#
# Copyright (C) 2014 Dragon Research Labs ("DRL")
# 
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
# 
# THE SOFTWARE IS PROVIDED "AS IS" AND DRL DISCLAIMS ALL WARRANTIES WITH
# REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
# AND FITNESS.  IN NO EVENT SHALL DRL BE LIABLE FOR ANY SPECIAL, DIRECT,
# INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
# LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
# OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
# PERFORMANCE OF THIS SOFTWARE.

"""
Key management tool for revised version of Sparta's BGPSEC implementation.
"""

import os
import sys
import argparse
import subprocess


default_keydir = os.getenv("BGPSEC_KEYDIR", ".")


class OpenSSLPipeline(object):
  """
  String together one or more OpenSSL commands in a pipeline, return
  stdout of the final command.  Callable object rather than function
  so we can instantiate it as a closure over the program arguments.
  """

  allowed_keywords = set(["input"])

  def __init__(self, args):
    self.args = args

  def __call__(self, *argses, **kwargs):
    assert all(kw in self.allowed_keywords for kw in kwargs)
    procs = []
    for args in argses:
      procs.append(subprocess.Popen((self.args.openssl_binary,) + args,
                                    stdout = subprocess.PIPE,
                                    stdin = procs[-1].stdout if procs else subprocess.PIPE))
    if "input" in kwargs:
      procs[0].stdin.write(kwargs["input"])
    procs[0].stdin.close()
    output = procs[-1].stdout.read()
    for i, proc in enumerate(procs):
      if proc.wait() != 0:
        raise subprocess.CalledProcessError(proc.returncode, argses[i][0])
    return output


def filename(args, skihex, suffix = ""):
  if args.simple_filenames:
    return "%s/%s%s" % (args.keydir, skihex, suffix)
  else:
    return "%s/%s/%s/%s%s" % (args.keydir, skihex[:2], skihex[2:6], skihex[6:], suffix)


def makedir(args, skihex):
  dn = os.path.dirname(filename(args, skihex))
  if not os.path.isdir(dn):
    if args.verbose:
      print "Creating directory", dn
    os.makedirs(dn)


def generate(args):
  """
  Generate an EC keypair, store in .private and .pub files named using
  the key's SKI value to generate the filenames.

  This code goes through some silly gymnastics using the old OpenSSL
  ecparam command instead of using the newer OpenSSL genpkey command,
  because we have to force the key into the required namedCurve form
  instead of explicitCurve.  OpenSSL itself doesn't much care, but
  since the SKI is defined as the SHA1 hash of the binary key value,
  using the wrong key encoding yields the wrong SKI value.
  """

  openssl = OpenSSLPipeline(args)
  pemkey = openssl(("ecparam", "-name", "prime256v1"),
                   ("ecparam", "-param_enc", "named_curve", "-genkey"))
  pemkey = pemkey.splitlines(True)
  pemkey = "".join(pemkey[pemkey.index("-----BEGIN EC PRIVATE KEY-----\n"):])
  skihex = openssl(("pkey", "-outform", "DER", "-pubout"),
                   ("dgst", "-sha1", "-hex"),
                   input = pemkey)
  skihex = skihex.split()[-1].upper()
  if args.printski:
    print skihex
  makedir(args, skihex)
  fn = filename(args, skihex, ".pub")
  if args.verbose:
    print "Writing", fn
  openssl(("pkey", "-outform", "DER", "-out", fn, "-pubout"), input = pemkey)
  os.umask(077)
  fn = filename(args, skihex, ".private")
  if args.verbose:
    print "Writing", fn
  openssl(("pkey", "-outform", "DER", "-out", fn), input = pemkey)


def hashdir(args):
  """
  Extract router keys from certificates in an RPKI certificate tree, store as .pub
  files using each key's SKI value to generate the corresponding filename.
  """

  openssl = OpenSSLPipeline(args)
  for root, dirs, files in os.walk(args.cert_dir):
    for fn in files:
      if fn.endswith(".cer"):
        fn = os.path.join(root, fn)
        text = openssl(("x509", "-inform", "DER", "-noout", "-text", "-in", fn))
        if "Public Key Algorithm: id-ecPublicKey" not in text or "ASN1 OID: prime256v1" not in text:
          continue
        skihex = text[text.index("X509v3 Subject Key Identifier:"):].splitlines()[1].strip().replace(":", "").upper()
        if args.paranoia:
          checkski = openssl(("x509", "-inform", "DER", "-noout", "-pubkey", "-in", fn),
                             ("pkey", "-pubin", "-outform", "DER"),
                             ("dgst", "-sha1", "-hex"))
          checkski = checkski.split()[-1].upper()
          if skihex != checkski:
            sys.stderr.write("SKI %s in certificate %s does not match calculated SKI %s\n" % (skihex, fn, checkski))
        makedir(args, skihex)
        outfn = filename(args, skihex, ".pub")
        if args.verbose:
          print "Writing", outfn
        openssl(("x509", "-inform", "DER", "-noout", "-pubkey", "-in", fn),
                ("pkey", "-pubin", "-outform", "DER", "-out", outfn))


def main():
  parser = argparse.ArgumentParser(description = __doc__)
  parser.add_argument("--openssl-binary",
                      default = "openssl",
                      help = "Path to EC-capable OpenSSL binary")
  parser.add_argument("--keydir",
                      default = default_keydir,
                      help = "directory to which we save router keys")
  parser.add_argument("--simple-filenames",
                      action = "store_true",
                      help = "don't use aa/bbcc/ddeeff... filenames")
  parser.add_argument("--verbose",
                      action = "store_true",
                      help = "whistle while you work")
  parser.add_argument("--printski",
                      action = "store_true",
                      help = "print out the SKI value")
  parser.add_argument("--paranoia",
                      action = "store_true",
                      help = "perform paranoid checks")
  subparsers = parser.add_subparsers(title = "Commands",
                                     metavar = "")
  subparser = subparsers.add_parser("generate",
                                    description = generate.__doc__,
                                    help = "generate new keypair")
  subparser.set_defaults(func = generate)
  #subparser.add_argument("router_id")
  #subparser.add_argument("asns", nargs = "+")
  subparser = subparsers.add_parser("hashdir",
                                    description = hashdir.__doc__,
                                    help = "hash directory of certs")
  subparser.set_defaults(func = hashdir)
  subparser.add_argument("cert_dir")
  args = parser.parse_args()
  return args.func(args)

if __name__ == "__main__":
  sys.exit(main())
