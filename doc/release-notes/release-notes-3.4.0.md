# Summary

This is a mandatory update which will hard fork the chain.

## How to Upgrade

If you are running an older version, shut it down. Wait until it has completely
shut down (which might take a few minutes for older versions), then run the
installer (on Windows) or just copy over /Applications/Galilel-Qt (on Mac) or
galileld/galilel-qt (on Linux).

## Compatibility

Galilel Core is extensively tested on multiple operating systems using the
Linux kernel, macOS 10.10+, and Windows 7 and later.

Microsoft ended support for Windows XP on April 8th, 2014, No attempt is made
to prevent installing or running the software on Windows XP, you can still do
so at your own risk but be aware that there are known instabilities and issues.
Please do not report issues about Windows XP to the issue tracker.

Apple released it's last Mountain Lion update August 13, 2015, and officially
ended support on December 14, 2015. Galilel Core software starting with v3.3.0
will no longer run on macOS versions prior to Yosemite (10.10). Please do not
report issues about macOS versions prior to Yosemite to the issue tracker.

Galilel Core should also work on most other Unix-like systems and is frequently
tested on Arch Linux, Fedora and Ubuntu.

## Notable Changes

### zGALI Public Spends

Recent exploits of the Zerocoin protocol (Wrapped serials and broken P1 proof)
required us to enable the zerocoin spork and deactivate zGALI functionality in
order to secure the supply until the pertinent review process was completed.

Moving forward from this undesired situation, we are enabling a secure and
chain storage friendly solution for the zerocoin public spend (aka zGALI to
GALI conversion).

The explanation of how this works can be found in [#27](https://github.com/Galilel-Project/galilel/pull/27)

After block `681,000` has past, `SPORK_16` will be deactivated to allow zGALI
spends to occur using this new public spend method for version 2 zGALI (version
1 zGALI is not in our chain, see note below). zGALI public spends, as the name
suggests, are **NOT** private, they reveal the input mint that is being spent.
The minting of **NEW** zGALI, as well as zGALI staking will remain disabled for
the time being.

It is advised that users spend/convert their existing zGALI to GALI, which can
be done via the GUI or RPC as it was prior to the disabling of zGALI. Note that
with the public spend method, the restriction on the number of denominations
per transaction (previously 7) has been lifted, and now allows for several
hundred denominations per transaction.

*Note on version 1* zGALI: Version 1 zGALI was never available in our chain
because we ported it from PIVX while v2 was already active.

Many thanks to furszy and random-zebra and the whole PIVX team for this
brilliant work.

### BIP65 (CHECKLOCKTIMEVERIFY) Soft-Fork

Galilel Core v3.4.0 introduces new consensus rules for scripting pathways to
support the [BIP65](https://github.com/bitcoin/bips/blob/master/bip-0065.mediawiki)
standard. This is being carried out as a soft-fork in order to provide ample
time for stakers to update their wallet version.

## GUI Changes

### Cloud Bootstrap

The wallet now supports fast bootstrap from the latest blockchain snapshot in
the cloud or from file. The snapshots are build inside data center on weekly
base. The default is to run bootstrap from the cloud, obviously user need to
trust our domain for valid blockchain bootstrap archive. However user can still
deselect it and run regular P2P blockchain sync.

You can also run the bootstrap from within the wallet under **Tools ->
Blockchain Bootstrap**. Also it is possible to bootstrap the blockchain via
`-bootstrap` parameter, useful for galileld running as daemon.

It is also possible to pass a `bootstrap.zip` archive on command line via
`-bootstrap=<filename>`, created by the user. This file must include the
following folders from a synchronized wallet:

```
blocks
chainstate
zerocoin
```

### Options Dialog Cleanup

The options/settings UI dialog has been cleaned up to no longer show settings
that are wallet related when running in "disable wallet" (`-disablewallet`)
mode.

### Privacy Tab

Notice text has been added to the privacy tab indicating that zGALI minting is
disabled, as well as the removal of UI elements that supported such
functionality. Notice text has also been added indicating that zGALI spends are
currently **NOT** private.

## RPC Changes

### Removal of Deprecated Commands

The `masternode` and `mnbudget` RPC commands, which were marked as deprecated
in PIVX Core v2.3.1 (September 19, 2017), have now been completely removed from
Galilel Core to align it with the upstream version. Several new commands were
added in previous version already to replace the two aforementioned commands,
reference the upstream PIVX [v2.3.1 Release Notes](https://github.com/PIVX-Project/PIVX/blob/master/doc/release-notes/release-notes-2.3.1.md#rpc-changes)
for further details.

### New `getblockindexstats` Command

A new RPC command (`getblockindexstats`) has been introduced which serves the
purpose of obtaining statistical information on a range of blocks. The
information returned is as follows:

* transaction count (not including coinbase/coinstake txes)
* transaction count (including coinbase/coinstake txes)
* zGALI per-denom mint count
* zGALI per-denom spend count
* total transaction bytes
* total fees in block range
* average fee per kB

Command Reference:

```
getblockindexstats height range ( fFeeOnly )
nReturns aggregated BlockIndex data for blocks
height, height+1, height+2, ..., height+range-1]

nArguments:
1. height             (numeric, required) block height where the search starts.
2. range              (numeric, required) number of blocks to include.
3. fFeeOnly           (boolean, optional, default=False) return only fee info.
```

Result:

```
{
  first_block: x,              (integer) First counted block
  last_block: x,               (integer) Last counted block
  txcount: xxxxx,              (numeric) tx count (excluding coinbase/coinstake)
  txcount_all: xxxxx,          (numeric) tx count (including coinbase/coinstake)
  mintcount: {              [if fFeeOnly=False]
        denom_1: xxxx,         (numeric) number of mints of denom_1 occurred over the block range
        denom_5: xxxx,         (numeric) number of mints of denom_5 occurred over the block range
         ...                    ... number of mints of other denominations: ..., 10, 50, 100, 500, 1000, 5000
  },
  spendcount: {             [if fFeeOnly=False]
        denom_1: xxxx,         (numeric) number of spends of denom_1 occurred over the block range
        denom_5: xxxx,         (numeric) number of spends of denom_5 occurred over the block range
         ...                    ... number of spends of other denominations: ..., 10, 50, 100, 500, 1000, 5000
  },
  pubspendcount: {          [if fFeeOnly=False]
        denom_1: xxxx,         (numeric) number of PUBLIC spends of denom_1 occurred over the block range
        denom_5: xxxx,         (numeric) number of PUBLIC spends of denom_5 occurred over the block range
         ...                   ... number of PUBLIC spends of other denominations: ..., 10, 50, 100, 500, 1000, 5000
  },
  txbytes: xxxxx,              (numeric) Sum of the size of all txes (zGALI excluded) over block range
  ttlfee: xxxxx,               (numeric) Sum of the fee amount of all txes (zGALI mints excluded) over block range
  ttlfee_all: xxxxx,           (numeric) Sum of the fee amount of all txes (zGALI mints included) over block range
  feeperkb: xxxxx,             (numeric) Average fee per kb (excluding zc txes)
}
```

## Build System Changes

### New Architectures for Depends

The depends system has new added support for the `s390x` and `ppc64el`
architectures. This is done in order to support the future integration with
[Snapcraft](https://www.snapcraft.io/), as well as to support any developers
who may use systems based on such architectures.

## Update Type

Optional | Soft fork | Hard fork
---------|-----------|----------
No       | No        | Yes

## Changelog v3.4.0

* New protocol version enforcement at Sun Aug 18 10:00:00 UTC 2019
* Enable Zerocoin with public spend of zGALI after block `681,000`
* Disabled Zerocoin minting and staking
* Bootstrap from the cloud or file
* Single transaction can include up to 500 denominations of public spends
* Fix for valid forked blocks rejected
* Preliminary additional architecture support (`s390x` and `ppc64el`)
* Performance increases

## Credits

* mbroemme
* ChristianGrieger
* presstab
* Fuzzbawls
* Mrs-X
* Warrows
* cevap
* furszy
* random-zebra
* pavik
