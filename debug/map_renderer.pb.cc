// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: map_renderer.proto

#include "map_renderer.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>

PROTOBUF_PRAGMA_INIT_SEG
namespace serial {
constexpr RenderSettings::RenderSettings(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : color_palette_()
  , bus_label_offset_(nullptr)
  , stop_label_offset_(nullptr)
  , underlayer_color_(nullptr)
  , width_(0)
  , height_(0)
  , padding_(0)
  , line_width_(0)
  , stop_radius_(0)
  , bus_label_font_size_(0u)
  , stop_label_font_size_(0u)
  , underlayer_width_(0){}
struct RenderSettingsDefaultTypeInternal {
  constexpr RenderSettingsDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~RenderSettingsDefaultTypeInternal() {}
  union {
    RenderSettings _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT RenderSettingsDefaultTypeInternal _RenderSettings_default_instance_;
}  // namespace serial
static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_map_5frenderer_2eproto[1];
static constexpr ::PROTOBUF_NAMESPACE_ID::EnumDescriptor const** file_level_enum_descriptors_map_5frenderer_2eproto = nullptr;
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_map_5frenderer_2eproto = nullptr;

const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_map_5frenderer_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::serial::RenderSettings, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::serial::RenderSettings, width_),
  PROTOBUF_FIELD_OFFSET(::serial::RenderSettings, height_),
  PROTOBUF_FIELD_OFFSET(::serial::RenderSettings, padding_),
  PROTOBUF_FIELD_OFFSET(::serial::RenderSettings, line_width_),
  PROTOBUF_FIELD_OFFSET(::serial::RenderSettings, stop_radius_),
  PROTOBUF_FIELD_OFFSET(::serial::RenderSettings, bus_label_font_size_),
  PROTOBUF_FIELD_OFFSET(::serial::RenderSettings, bus_label_offset_),
  PROTOBUF_FIELD_OFFSET(::serial::RenderSettings, stop_label_font_size_),
  PROTOBUF_FIELD_OFFSET(::serial::RenderSettings, stop_label_offset_),
  PROTOBUF_FIELD_OFFSET(::serial::RenderSettings, underlayer_color_),
  PROTOBUF_FIELD_OFFSET(::serial::RenderSettings, underlayer_width_),
  PROTOBUF_FIELD_OFFSET(::serial::RenderSettings, color_palette_),
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::serial::RenderSettings)},
};

static ::PROTOBUF_NAMESPACE_ID::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::serial::_RenderSettings_default_instance_),
};

const char descriptor_table_protodef_map_5frenderer_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\022map_renderer.proto\022\006serial\032\tsvg.proto\""
  "\342\002\n\016RenderSettings\022\r\n\005width\030\001 \001(\001\022\016\n\006hei"
  "ght\030\002 \001(\001\022\017\n\007padding\030\003 \001(\001\022\022\n\nline_width"
  "\030\004 \001(\001\022\023\n\013stop_radius\030\005 \001(\001\022\033\n\023bus_label"
  "_font_size\030\006 \001(\r\022(\n\020bus_label_offset\030\007 \001"
  "(\0132\016.serial.Offset\022\034\n\024stop_label_font_si"
  "ze\030\010 \001(\r\022)\n\021stop_label_offset\030\t \001(\0132\016.se"
  "rial.Offset\022\'\n\020underlayer_color\030\n \001(\0132\r."
  "serial.Color\022\030\n\020underlayer_width\030\013 \001(\001\022$"
  "\n\rcolor_palette\030\014 \003(\0132\r.serial.Colorb\006pr"
  "oto3"
  ;
static const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable*const descriptor_table_map_5frenderer_2eproto_deps[1] = {
  &::descriptor_table_svg_2eproto,
};
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_map_5frenderer_2eproto_once;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_map_5frenderer_2eproto = {
  false, false, 404, descriptor_table_protodef_map_5frenderer_2eproto, "map_renderer.proto", 
  &descriptor_table_map_5frenderer_2eproto_once, descriptor_table_map_5frenderer_2eproto_deps, 1, 1,
  schemas, file_default_instances, TableStruct_map_5frenderer_2eproto::offsets,
  file_level_metadata_map_5frenderer_2eproto, file_level_enum_descriptors_map_5frenderer_2eproto, file_level_service_descriptors_map_5frenderer_2eproto,
};
PROTOBUF_ATTRIBUTE_WEAK const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable* descriptor_table_map_5frenderer_2eproto_getter() {
  return &descriptor_table_map_5frenderer_2eproto;
}

// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY static ::PROTOBUF_NAMESPACE_ID::internal::AddDescriptorsRunner dynamic_init_dummy_map_5frenderer_2eproto(&descriptor_table_map_5frenderer_2eproto);
namespace serial {

// ===================================================================

class RenderSettings::_Internal {
 public:
  static const ::serial::Offset& bus_label_offset(const RenderSettings* msg);
  static const ::serial::Offset& stop_label_offset(const RenderSettings* msg);
  static const ::serial::Color& underlayer_color(const RenderSettings* msg);
};

const ::serial::Offset&
RenderSettings::_Internal::bus_label_offset(const RenderSettings* msg) {
  return *msg->bus_label_offset_;
}
const ::serial::Offset&
RenderSettings::_Internal::stop_label_offset(const RenderSettings* msg) {
  return *msg->stop_label_offset_;
}
const ::serial::Color&
RenderSettings::_Internal::underlayer_color(const RenderSettings* msg) {
  return *msg->underlayer_color_;
}
void RenderSettings::clear_bus_label_offset() {
  if (GetArenaForAllocation() == nullptr && bus_label_offset_ != nullptr) {
    delete bus_label_offset_;
  }
  bus_label_offset_ = nullptr;
}
void RenderSettings::clear_stop_label_offset() {
  if (GetArenaForAllocation() == nullptr && stop_label_offset_ != nullptr) {
    delete stop_label_offset_;
  }
  stop_label_offset_ = nullptr;
}
void RenderSettings::clear_underlayer_color() {
  if (GetArenaForAllocation() == nullptr && underlayer_color_ != nullptr) {
    delete underlayer_color_;
  }
  underlayer_color_ = nullptr;
}
void RenderSettings::clear_color_palette() {
  color_palette_.Clear();
}
RenderSettings::RenderSettings(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena, is_message_owned),
  color_palette_(arena) {
  SharedCtor();
  if (!is_message_owned) {
    RegisterArenaDtor(arena);
  }
  // @@protoc_insertion_point(arena_constructor:serial.RenderSettings)
}
RenderSettings::RenderSettings(const RenderSettings& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      color_palette_(from.color_palette_) {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  if (from._internal_has_bus_label_offset()) {
    bus_label_offset_ = new ::serial::Offset(*from.bus_label_offset_);
  } else {
    bus_label_offset_ = nullptr;
  }
  if (from._internal_has_stop_label_offset()) {
    stop_label_offset_ = new ::serial::Offset(*from.stop_label_offset_);
  } else {
    stop_label_offset_ = nullptr;
  }
  if (from._internal_has_underlayer_color()) {
    underlayer_color_ = new ::serial::Color(*from.underlayer_color_);
  } else {
    underlayer_color_ = nullptr;
  }
  ::memcpy(&width_, &from.width_,
    static_cast<size_t>(reinterpret_cast<char*>(&underlayer_width_) -
    reinterpret_cast<char*>(&width_)) + sizeof(underlayer_width_));
  // @@protoc_insertion_point(copy_constructor:serial.RenderSettings)
}

inline void RenderSettings::SharedCtor() {
::memset(reinterpret_cast<char*>(this) + static_cast<size_t>(
    reinterpret_cast<char*>(&bus_label_offset_) - reinterpret_cast<char*>(this)),
    0, static_cast<size_t>(reinterpret_cast<char*>(&underlayer_width_) -
    reinterpret_cast<char*>(&bus_label_offset_)) + sizeof(underlayer_width_));
}

RenderSettings::~RenderSettings() {
  // @@protoc_insertion_point(destructor:serial.RenderSettings)
  if (GetArenaForAllocation() != nullptr) return;
  SharedDtor();
  _internal_metadata_.Delete<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

inline void RenderSettings::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
  if (this != internal_default_instance()) delete bus_label_offset_;
  if (this != internal_default_instance()) delete stop_label_offset_;
  if (this != internal_default_instance()) delete underlayer_color_;
}

void RenderSettings::ArenaDtor(void* object) {
  RenderSettings* _this = reinterpret_cast< RenderSettings* >(object);
  (void)_this;
}
void RenderSettings::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void RenderSettings::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void RenderSettings::Clear() {
// @@protoc_insertion_point(message_clear_start:serial.RenderSettings)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  color_palette_.Clear();
  if (GetArenaForAllocation() == nullptr && bus_label_offset_ != nullptr) {
    delete bus_label_offset_;
  }
  bus_label_offset_ = nullptr;
  if (GetArenaForAllocation() == nullptr && stop_label_offset_ != nullptr) {
    delete stop_label_offset_;
  }
  stop_label_offset_ = nullptr;
  if (GetArenaForAllocation() == nullptr && underlayer_color_ != nullptr) {
    delete underlayer_color_;
  }
  underlayer_color_ = nullptr;
  ::memset(&width_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&underlayer_width_) -
      reinterpret_cast<char*>(&width_)) + sizeof(underlayer_width_));
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* RenderSettings::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // double width = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 9)) {
          width_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<double>(ptr);
          ptr += sizeof(double);
        } else goto handle_unusual;
        continue;
      // double height = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 17)) {
          height_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<double>(ptr);
          ptr += sizeof(double);
        } else goto handle_unusual;
        continue;
      // double padding = 3;
      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 25)) {
          padding_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<double>(ptr);
          ptr += sizeof(double);
        } else goto handle_unusual;
        continue;
      // double line_width = 4;
      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 33)) {
          line_width_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<double>(ptr);
          ptr += sizeof(double);
        } else goto handle_unusual;
        continue;
      // double stop_radius = 5;
      case 5:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 41)) {
          stop_radius_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<double>(ptr);
          ptr += sizeof(double);
        } else goto handle_unusual;
        continue;
      // uint32 bus_label_font_size = 6;
      case 6:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 48)) {
          bus_label_font_size_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // .serial.Offset bus_label_offset = 7;
      case 7:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 58)) {
          ptr = ctx->ParseMessage(_internal_mutable_bus_label_offset(), ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // uint32 stop_label_font_size = 8;
      case 8:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 64)) {
          stop_label_font_size_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // .serial.Offset stop_label_offset = 9;
      case 9:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 74)) {
          ptr = ctx->ParseMessage(_internal_mutable_stop_label_offset(), ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // .serial.Color underlayer_color = 10;
      case 10:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 82)) {
          ptr = ctx->ParseMessage(_internal_mutable_underlayer_color(), ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // double underlayer_width = 11;
      case 11:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 89)) {
          underlayer_width_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<double>(ptr);
          ptr += sizeof(double);
        } else goto handle_unusual;
        continue;
      // repeated .serial.Color color_palette = 12;
      case 12:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 98)) {
          ptr -= 1;
          do {
            ptr += 1;
            ptr = ctx->ParseMessage(_internal_add_color_palette(), ptr);
            CHK_(ptr);
            if (!ctx->DataAvailable(ptr)) break;
          } while (::PROTOBUF_NAMESPACE_ID::internal::ExpectTag<98>(ptr));
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag == 0) || ((tag & 7) == 4)) {
          CHK_(ptr);
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* RenderSettings::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:serial.RenderSettings)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // double width = 1;
  if (!(this->_internal_width() <= 0 && this->_internal_width() >= 0)) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteDoubleToArray(1, this->_internal_width(), target);
  }

  // double height = 2;
  if (!(this->_internal_height() <= 0 && this->_internal_height() >= 0)) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteDoubleToArray(2, this->_internal_height(), target);
  }

  // double padding = 3;
  if (!(this->_internal_padding() <= 0 && this->_internal_padding() >= 0)) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteDoubleToArray(3, this->_internal_padding(), target);
  }

  // double line_width = 4;
  if (!(this->_internal_line_width() <= 0 && this->_internal_line_width() >= 0)) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteDoubleToArray(4, this->_internal_line_width(), target);
  }

  // double stop_radius = 5;
  if (!(this->_internal_stop_radius() <= 0 && this->_internal_stop_radius() >= 0)) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteDoubleToArray(5, this->_internal_stop_radius(), target);
  }

  // uint32 bus_label_font_size = 6;
  if (this->_internal_bus_label_font_size() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(6, this->_internal_bus_label_font_size(), target);
  }

  // .serial.Offset bus_label_offset = 7;
  if (this->_internal_has_bus_label_offset()) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(
        7, _Internal::bus_label_offset(this), target, stream);
  }

  // uint32 stop_label_font_size = 8;
  if (this->_internal_stop_label_font_size() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(8, this->_internal_stop_label_font_size(), target);
  }

  // .serial.Offset stop_label_offset = 9;
  if (this->_internal_has_stop_label_offset()) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(
        9, _Internal::stop_label_offset(this), target, stream);
  }

  // .serial.Color underlayer_color = 10;
  if (this->_internal_has_underlayer_color()) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(
        10, _Internal::underlayer_color(this), target, stream);
  }

  // double underlayer_width = 11;
  if (!(this->_internal_underlayer_width() <= 0 && this->_internal_underlayer_width() >= 0)) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteDoubleToArray(11, this->_internal_underlayer_width(), target);
  }

  // repeated .serial.Color color_palette = 12;
  for (unsigned int i = 0,
      n = static_cast<unsigned int>(this->_internal_color_palette_size()); i < n; i++) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(12, this->_internal_color_palette(i), target, stream);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:serial.RenderSettings)
  return target;
}

size_t RenderSettings::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:serial.RenderSettings)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // repeated .serial.Color color_palette = 12;
  total_size += 1UL * this->_internal_color_palette_size();
  for (const auto& msg : this->color_palette_) {
    total_size +=
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(msg);
  }

  // .serial.Offset bus_label_offset = 7;
  if (this->_internal_has_bus_label_offset()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(
        *bus_label_offset_);
  }

  // .serial.Offset stop_label_offset = 9;
  if (this->_internal_has_stop_label_offset()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(
        *stop_label_offset_);
  }

  // .serial.Color underlayer_color = 10;
  if (this->_internal_has_underlayer_color()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(
        *underlayer_color_);
  }

  // double width = 1;
  if (!(this->_internal_width() <= 0 && this->_internal_width() >= 0)) {
    total_size += 1 + 8;
  }

  // double height = 2;
  if (!(this->_internal_height() <= 0 && this->_internal_height() >= 0)) {
    total_size += 1 + 8;
  }

  // double padding = 3;
  if (!(this->_internal_padding() <= 0 && this->_internal_padding() >= 0)) {
    total_size += 1 + 8;
  }

  // double line_width = 4;
  if (!(this->_internal_line_width() <= 0 && this->_internal_line_width() >= 0)) {
    total_size += 1 + 8;
  }

  // double stop_radius = 5;
  if (!(this->_internal_stop_radius() <= 0 && this->_internal_stop_radius() >= 0)) {
    total_size += 1 + 8;
  }

  // uint32 bus_label_font_size = 6;
  if (this->_internal_bus_label_font_size() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
        this->_internal_bus_label_font_size());
  }

  // uint32 stop_label_font_size = 8;
  if (this->_internal_stop_label_font_size() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
        this->_internal_stop_label_font_size());
  }

  // double underlayer_width = 11;
  if (!(this->_internal_underlayer_width() <= 0 && this->_internal_underlayer_width() >= 0)) {
    total_size += 1 + 8;
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

const ::PROTOBUF_NAMESPACE_ID::Message::ClassData RenderSettings::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::Message::CopyWithSizeCheck,
    RenderSettings::MergeImpl
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*RenderSettings::GetClassData() const { return &_class_data_; }

void RenderSettings::MergeImpl(::PROTOBUF_NAMESPACE_ID::Message*to,
                      const ::PROTOBUF_NAMESPACE_ID::Message&from) {
  static_cast<RenderSettings *>(to)->MergeFrom(
      static_cast<const RenderSettings &>(from));
}


void RenderSettings::MergeFrom(const RenderSettings& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:serial.RenderSettings)
  GOOGLE_DCHECK_NE(&from, this);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  color_palette_.MergeFrom(from.color_palette_);
  if (from._internal_has_bus_label_offset()) {
    _internal_mutable_bus_label_offset()->::serial::Offset::MergeFrom(from._internal_bus_label_offset());
  }
  if (from._internal_has_stop_label_offset()) {
    _internal_mutable_stop_label_offset()->::serial::Offset::MergeFrom(from._internal_stop_label_offset());
  }
  if (from._internal_has_underlayer_color()) {
    _internal_mutable_underlayer_color()->::serial::Color::MergeFrom(from._internal_underlayer_color());
  }
  if (!(from._internal_width() <= 0 && from._internal_width() >= 0)) {
    _internal_set_width(from._internal_width());
  }
  if (!(from._internal_height() <= 0 && from._internal_height() >= 0)) {
    _internal_set_height(from._internal_height());
  }
  if (!(from._internal_padding() <= 0 && from._internal_padding() >= 0)) {
    _internal_set_padding(from._internal_padding());
  }
  if (!(from._internal_line_width() <= 0 && from._internal_line_width() >= 0)) {
    _internal_set_line_width(from._internal_line_width());
  }
  if (!(from._internal_stop_radius() <= 0 && from._internal_stop_radius() >= 0)) {
    _internal_set_stop_radius(from._internal_stop_radius());
  }
  if (from._internal_bus_label_font_size() != 0) {
    _internal_set_bus_label_font_size(from._internal_bus_label_font_size());
  }
  if (from._internal_stop_label_font_size() != 0) {
    _internal_set_stop_label_font_size(from._internal_stop_label_font_size());
  }
  if (!(from._internal_underlayer_width() <= 0 && from._internal_underlayer_width() >= 0)) {
    _internal_set_underlayer_width(from._internal_underlayer_width());
  }
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
}

void RenderSettings::CopyFrom(const RenderSettings& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:serial.RenderSettings)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool RenderSettings::IsInitialized() const {
  return true;
}

void RenderSettings::InternalSwap(RenderSettings* other) {
  using std::swap;
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  color_palette_.InternalSwap(&other->color_palette_);
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(RenderSettings, underlayer_width_)
      + sizeof(RenderSettings::underlayer_width_)
      - PROTOBUF_FIELD_OFFSET(RenderSettings, bus_label_offset_)>(
          reinterpret_cast<char*>(&bus_label_offset_),
          reinterpret_cast<char*>(&other->bus_label_offset_));
}

::PROTOBUF_NAMESPACE_ID::Metadata RenderSettings::GetMetadata() const {
  return ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(
      &descriptor_table_map_5frenderer_2eproto_getter, &descriptor_table_map_5frenderer_2eproto_once,
      file_level_metadata_map_5frenderer_2eproto[0]);
}

// @@protoc_insertion_point(namespace_scope)
}  // namespace serial
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::serial::RenderSettings* Arena::CreateMaybeMessage< ::serial::RenderSettings >(Arena* arena) {
  return Arena::CreateMessageInternal< ::serial::RenderSettings >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>