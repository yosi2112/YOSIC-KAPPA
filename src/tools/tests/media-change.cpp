#include "cli/disks.hpp"
#include <chrono>
#include <iostream>
#include <stdexcept>

namespace {
void check(bool result, const char *message) {
    if (!result)
        throw std::runtime_error(message);
}
template <typename Action> void fails(Action action) {
    try {
        action();
    } catch (const std::exception &) {
        return;
    }
    throw std::runtime_error("expected media operation to fail");
}
} // namespace
int main() {
    try {
        const auto directory =
            std::filesystem::path("out") /
            ("Media Swap Test " +
             std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
        std::filesystem::create_directories(directory);
        auto first = yosic::media::create({}, 0x11);
        auto second = yosic::media::create({}, 0x22);
        const auto a = directory / "MixedCase First.d88", b = directory / "Second.d88";
        const auto raw = directory / "disk.img", internal = directory / "disk.kfd";
        const auto bad = directory / "broken.d88", output = directory / "saved.d88";
        yosic::media::save_new(a, yosic::media::write_d88({{first, second}}));
        yosic::media::save_new(b, yosic::media::write_d88({{second}}));
        yosic::media::save_new(raw, yosic::media::write_raw(second, {}));
        yosic::media::save_new(internal, yosic::media::write_internal(first));
        yosic::media::save_new(bad, yosic::media::Bytes{0, 1, 2});
        yosic::Clock clock;
        yosic::Fdc fdc(clock);
        yosic::FdcBoard board(fdc);
        yosic::DiskMounts mounts;
        check(!mounts.command("R", fdc), "non-disk commands pass through");
        auto text = "  aT 0 \"" + a.string() + "\"";
        check(mounts.command(text, fdc)->find(a.string()) != std::string::npos,
              "AT preserves quoted filename case and spaces");
        check(fdc.drives[0].read_only && fdc.drives[0].disk->tracks[0].sectors[0].data[0] == 0x11,
              "first D88 disk attached read-only");
        const auto old = fdc.drives[0].disk;
        const auto generation = board.read(32);
        fails([&] { mounts.command("AT 0 \"" + bad.string() + "\"", fdc); });
        fails([&] { mounts.command("AT 4 \"" + b.string() + "\"", fdc); });
        fails([&] { mounts.command("AT 0 \"missing.d88\"", fdc); });
        fails([&] { mounts.command("AT 0 \"", fdc); });
        check(fdc.drives[0].disk == old && board.peek(32) == generation,
              "failed replacement leaves old medium and generation intact");
        board.write(20, 1);
        fails([&] { mounts.attach(fdc, 0, b); });
        mounts.attach(fdc, 1, b);
        fails([&] { mounts.command("DT ALL", fdc); });
        check(fdc.drives[0].disk == old && fdc.drives[1].disk, "DT ALL lock preflight");
        board.write(20, 0);
        fails([&] { board.write(20, 16); });
        board.write(4, 3);
        fails([&] { mounts.detach(fdc, 0); });
        board.write(4, 0xdf);
        board.write(4, 1);
        check(fdc.phase() == yosic::Fdc::Phase::idle, "complete SPECIFY");
        mounts.command("AT -R FD0 \"" + b.string() + "\"", fdc);
        check(board.read(32) == generation + 1 &&
                  fdc.drives[0].disk->tracks[0].sectors[0].data[0] == 0x22,
              "successful replacement updates media generation");
        mounts.command("DT 0", fdc);
        check(!fdc.drives[0].disk && board.read(32) == generation + 2, "DT disconnects medium");
        mounts.attach(fdc, 0, raw);
        check(fdc.drives[0].disk->tracks[0].sectors[0].data[0] == 0x22,
              "raw attach default geometry");
        mounts.attach(fdc, 0, internal);
        check(fdc.drives[0].disk->tracks[0].sectors[0].data[0] == 0x11, "KFD attach");
        check(mounts.command("AT", fdc)->find("FD3: not attached") != std::string::npos,
              "AT displays all attachment paths");
        mounts.command("DT ALL", fdc);
        check(!fdc.drives[0].disk && !fdc.drives[1].disk, "detach all");

        yosic::DiskMounts writable;
        auto parse = [&](std::string_view option, const std::vector<std::string> &args) {
            unsigned i = 0;
            check(writable.parse(option, [&]() -> std::string_view { return args.at(i++); }),
                  "disk option parse");
        };
        parse("--disk", {"0", a.string()});
        parse("--disk-index", {"0", "1"});
        parse("--disk-copy", {"0", output.string()});
        writable.validate();
        writable.load(fdc);
        fdc.drives[0].disk->tracks[0].sectors[0].data[0] = 0x77;
        fdc.drives[0].disk->dirty = true;
        const auto dirty = fdc.drives[0].disk;
        fails([&] { writable.attach(fdc, 0, bad); });
        check(!std::filesystem::exists(output) && fdc.drives[0].disk == dirty,
              "invalid replacement does not save or discard pending writes");
        yosic::media::save_new(output, yosic::media::Bytes{0});
        fails([&] { writable.attach(fdc, 0, b); });
        check(fdc.drives[0].disk == dirty && dirty->dirty, "save failure keeps old medium");
        std::filesystem::remove(output);
        writable.attach(fdc, 0, b);
        const auto saved = yosic::media::read_d88(yosic::media::load(output));
        check(saved.disks.size() == 2 && saved.disks[0].tracks[0].sectors[0].data[0] == 0x11 &&
                  saved.disks[1].tracks[0].sectors[0].data[0] == 0x77,
              "old writable copy saved with other D88 members preserved");
        check(yosic::media::load(a) == yosic::media::write_d88({{first, second}}),
              "original source is unchanged");
        writable.save(fdc); // Cleared old output must not be written again on exit.
        check(fdc.drives[0].read_only, "replacement has separate read-only policy");
        std::cout << "PASS AT/DT: formats, quotes, generation, locks, failure rollback, "
                     "saved copies and multi-disk D88 preservation\n";
        return 0;
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
