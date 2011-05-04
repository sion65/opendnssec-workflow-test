// Generated by the protocol buffer compiler.  DO NOT EDIT!

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "hsmkey.pb.h"
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>

namespace ods {
namespace hsmkey {

namespace {

const ::google::protobuf::Descriptor* HsmKeyDocument_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  HsmKeyDocument_reflection_ = NULL;
const ::google::protobuf::Descriptor* HsmKey_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  HsmKey_reflection_ = NULL;
const ::google::protobuf::EnumDescriptor* keyrole_descriptor_ = NULL;

}  // namespace


void protobuf_AssignDesc_hsmkey_2eproto() {
  protobuf_AddDesc_hsmkey_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "hsmkey.proto");
  GOOGLE_CHECK(file != NULL);
  HsmKeyDocument_descriptor_ = file->message_type(0);
  static const int HsmKeyDocument_offsets_[1] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(HsmKeyDocument, keys_),
  };
  HsmKeyDocument_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      HsmKeyDocument_descriptor_,
      HsmKeyDocument::default_instance_,
      HsmKeyDocument_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(HsmKeyDocument, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(HsmKeyDocument, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(HsmKeyDocument));
  HsmKey_descriptor_ = file->message_type(1);
  static const int HsmKey_offsets_[11] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(HsmKey, locator_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(HsmKey, candidate_for_sharing_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(HsmKey, bits_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(HsmKey, policy_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(HsmKey, algorithm_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(HsmKey, role_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(HsmKey, used_by_zones_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(HsmKey, inception_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(HsmKey, revoke_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(HsmKey, algorithm_name_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(HsmKey, hsm_name_),
  };
  HsmKey_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      HsmKey_descriptor_,
      HsmKey::default_instance_,
      HsmKey_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(HsmKey, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(HsmKey, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(HsmKey));
  keyrole_descriptor_ = file->enum_type(0);
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_hsmkey_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    HsmKeyDocument_descriptor_, &HsmKeyDocument::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    HsmKey_descriptor_, &HsmKey::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_hsmkey_2eproto() {
  delete HsmKeyDocument::default_instance_;
  delete HsmKeyDocument_reflection_;
  delete HsmKey::default_instance_;
  delete HsmKey_reflection_;
}

void protobuf_AddDesc_hsmkey_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::protobuf_AddDesc_xmlext_2eproto();
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\014hsmkey.proto\022\nods.hsmkey\032\014xmlext.proto"
    "\"2\n\016HsmKeyDocument\022 \n\004keys\030\001 \003(\0132\022.ods.h"
    "smkey.HsmKey\"\225\002\n\006HsmKey\022\017\n\007locator\030\001 \002(\t"
    "\022$\n\025candidate_for_sharing\030\002 \001(\010:\005false\022\022"
    "\n\004bits\030\003 \001(\r:\0042048\022\027\n\006policy\030\004 \001(\t:\007defa"
    "ult\022\024\n\talgorithm\030\005 \001(\r:\0011\022&\n\004role\030\006 \001(\0162"
    "\023.ods.hsmkey.keyrole:\003ZSK\022\025\n\rused_by_zon"
    "es\030\007 \003(\t\022\021\n\tinception\030\010 \001(\r\022\025\n\006revoke\030\t "
    "\001(\010:\005false\022\026\n\016algorithm_name\030\n \001(\t\022\020\n\010hs"
    "m_name\030\013 \001(\t*$\n\007keyrole\022\007\n\003KSK\020\001\022\007\n\003ZSK\020"
    "\002\022\007\n\003CSK\020\003", 410);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "hsmkey.proto", &protobuf_RegisterTypes);
  HsmKeyDocument::default_instance_ = new HsmKeyDocument();
  HsmKey::default_instance_ = new HsmKey();
  HsmKeyDocument::default_instance_->InitAsDefaultInstance();
  HsmKey::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_hsmkey_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_hsmkey_2eproto {
  StaticDescriptorInitializer_hsmkey_2eproto() {
    protobuf_AddDesc_hsmkey_2eproto();
  }
} static_descriptor_initializer_hsmkey_2eproto_;

const ::google::protobuf::EnumDescriptor* keyrole_descriptor() {
  protobuf_AssignDescriptorsOnce();
  return keyrole_descriptor_;
}
bool keyrole_IsValid(int value) {
  switch(value) {
    case 1:
    case 2:
    case 3:
      return true;
    default:
      return false;
  }
}


// ===================================================================

#ifndef _MSC_VER
const int HsmKeyDocument::kKeysFieldNumber;
#endif  // !_MSC_VER

HsmKeyDocument::HsmKeyDocument() {
  SharedCtor();
}

void HsmKeyDocument::InitAsDefaultInstance() {
}

HsmKeyDocument::HsmKeyDocument(const HsmKeyDocument& from) {
  SharedCtor();
  MergeFrom(from);
}

void HsmKeyDocument::SharedCtor() {
  _cached_size_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

HsmKeyDocument::~HsmKeyDocument() {
  SharedDtor();
}

void HsmKeyDocument::SharedDtor() {
  if (this != default_instance_) {
  }
}

const ::google::protobuf::Descriptor* HsmKeyDocument::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return HsmKeyDocument_descriptor_;
}

const HsmKeyDocument& HsmKeyDocument::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_hsmkey_2eproto();  return *default_instance_;
}

HsmKeyDocument* HsmKeyDocument::default_instance_ = NULL;

HsmKeyDocument* HsmKeyDocument::New() const {
  return new HsmKeyDocument;
}

void HsmKeyDocument::Clear() {
  keys_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool HsmKeyDocument::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // repeated .ods.hsmkey.HsmKey keys = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) !=
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          goto handle_uninterpreted;
        }
       parse_keys:
        DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
              input, add_keys()));
        if (input->ExpectTag(10)) goto parse_keys;
        if (input->ExpectAtEnd()) return true;
        break;
      }
      
      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void HsmKeyDocument::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  ::google::protobuf::uint8* raw_buffer = output->GetDirectBufferForNBytesAndAdvance(_cached_size_);
  if (raw_buffer != NULL) {
    HsmKeyDocument::SerializeWithCachedSizesToArray(raw_buffer);
    return;
  }
  
  // repeated .ods.hsmkey.HsmKey keys = 1;
  for (int i = 0; i < this->keys_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageNoVirtual(
      1, this->keys(i), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* HsmKeyDocument::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // repeated .ods.hsmkey.HsmKey keys = 1;
  for (int i = 0; i < this->keys_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        1, this->keys(i), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int HsmKeyDocument::ByteSize() const {
  int total_size = 0;
  
  // repeated .ods.hsmkey.HsmKey keys = 1;
  total_size += 1 * this->keys_size();
  for (int i = 0; i < this->keys_size(); i++) {
    total_size +=
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        this->keys(i));
  }
  
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  _cached_size_ = total_size;
  return total_size;
}

void HsmKeyDocument::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const HsmKeyDocument* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const HsmKeyDocument*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void HsmKeyDocument::MergeFrom(const HsmKeyDocument& from) {
  GOOGLE_CHECK_NE(&from, this);
  keys_.MergeFrom(from.keys_);
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void HsmKeyDocument::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void HsmKeyDocument::CopyFrom(const HsmKeyDocument& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool HsmKeyDocument::IsInitialized() const {
  
  for (int i = 0; i < keys_size(); i++) {
    if (!this->keys(i).IsInitialized()) return false;
  }
  return true;
}

void HsmKeyDocument::Swap(HsmKeyDocument* other) {
  if (other != this) {
    keys_.Swap(&other->keys_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata HsmKeyDocument::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = HsmKeyDocument_descriptor_;
  metadata.reflection = HsmKeyDocument_reflection_;
  return metadata;
}


// ===================================================================

const ::std::string HsmKey::_default_locator_;
const ::std::string HsmKey::_default_policy_("default");
const ::std::string HsmKey::_default_algorithm_name_;
const ::std::string HsmKey::_default_hsm_name_;
#ifndef _MSC_VER
const int HsmKey::kLocatorFieldNumber;
const int HsmKey::kCandidateForSharingFieldNumber;
const int HsmKey::kBitsFieldNumber;
const int HsmKey::kPolicyFieldNumber;
const int HsmKey::kAlgorithmFieldNumber;
const int HsmKey::kRoleFieldNumber;
const int HsmKey::kUsedByZonesFieldNumber;
const int HsmKey::kInceptionFieldNumber;
const int HsmKey::kRevokeFieldNumber;
const int HsmKey::kAlgorithmNameFieldNumber;
const int HsmKey::kHsmNameFieldNumber;
#endif  // !_MSC_VER

HsmKey::HsmKey() {
  SharedCtor();
}

void HsmKey::InitAsDefaultInstance() {
}

HsmKey::HsmKey(const HsmKey& from) {
  SharedCtor();
  MergeFrom(from);
}

void HsmKey::SharedCtor() {
  _cached_size_ = 0;
  locator_ = const_cast< ::std::string*>(&_default_locator_);
  candidate_for_sharing_ = false;
  bits_ = 2048u;
  policy_ = const_cast< ::std::string*>(&_default_policy_);
  algorithm_ = 1u;
  role_ = 2;
  inception_ = 0u;
  revoke_ = false;
  algorithm_name_ = const_cast< ::std::string*>(&_default_algorithm_name_);
  hsm_name_ = const_cast< ::std::string*>(&_default_hsm_name_);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

HsmKey::~HsmKey() {
  SharedDtor();
}

void HsmKey::SharedDtor() {
  if (locator_ != &_default_locator_) {
    delete locator_;
  }
  if (policy_ != &_default_policy_) {
    delete policy_;
  }
  if (algorithm_name_ != &_default_algorithm_name_) {
    delete algorithm_name_;
  }
  if (hsm_name_ != &_default_hsm_name_) {
    delete hsm_name_;
  }
  if (this != default_instance_) {
  }
}

const ::google::protobuf::Descriptor* HsmKey::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return HsmKey_descriptor_;
}

const HsmKey& HsmKey::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_hsmkey_2eproto();  return *default_instance_;
}

HsmKey* HsmKey::default_instance_ = NULL;

HsmKey* HsmKey::New() const {
  return new HsmKey;
}

void HsmKey::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (_has_bit(0)) {
      if (locator_ != &_default_locator_) {
        locator_->clear();
      }
    }
    candidate_for_sharing_ = false;
    bits_ = 2048u;
    if (_has_bit(3)) {
      if (policy_ != &_default_policy_) {
        policy_->assign(_default_policy_);
      }
    }
    algorithm_ = 1u;
    role_ = 2;
    inception_ = 0u;
  }
  if (_has_bits_[8 / 32] & (0xffu << (8 % 32))) {
    revoke_ = false;
    if (_has_bit(9)) {
      if (algorithm_name_ != &_default_algorithm_name_) {
        algorithm_name_->clear();
      }
    }
    if (_has_bit(10)) {
      if (hsm_name_ != &_default_hsm_name_) {
        hsm_name_->clear();
      }
    }
  }
  used_by_zones_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool HsmKey::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required string locator = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) !=
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          goto handle_uninterpreted;
        }
        DO_(::google::protobuf::internal::WireFormatLite::ReadString(
              input, this->mutable_locator()));
        ::google::protobuf::internal::WireFormat::VerifyUTF8String(
          this->locator().data(), this->locator().length(),
          ::google::protobuf::internal::WireFormat::PARSE);
        if (input->ExpectTag(16)) goto parse_candidate_for_sharing;
        break;
      }
      
      // optional bool candidate_for_sharing = 2 [default = false];
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) !=
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          goto handle_uninterpreted;
        }
       parse_candidate_for_sharing:
        DO_(::google::protobuf::internal::WireFormatLite::ReadBool(
              input, &candidate_for_sharing_));
        _set_bit(1);
        if (input->ExpectTag(24)) goto parse_bits;
        break;
      }
      
      // optional uint32 bits = 3 [default = 2048];
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) !=
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          goto handle_uninterpreted;
        }
       parse_bits:
        DO_(::google::protobuf::internal::WireFormatLite::ReadUInt32(
              input, &bits_));
        _set_bit(2);
        if (input->ExpectTag(34)) goto parse_policy;
        break;
      }
      
      // optional string policy = 4 [default = "default"];
      case 4: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) !=
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          goto handle_uninterpreted;
        }
       parse_policy:
        DO_(::google::protobuf::internal::WireFormatLite::ReadString(
              input, this->mutable_policy()));
        ::google::protobuf::internal::WireFormat::VerifyUTF8String(
          this->policy().data(), this->policy().length(),
          ::google::protobuf::internal::WireFormat::PARSE);
        if (input->ExpectTag(40)) goto parse_algorithm;
        break;
      }
      
      // optional uint32 algorithm = 5 [default = 1];
      case 5: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) !=
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          goto handle_uninterpreted;
        }
       parse_algorithm:
        DO_(::google::protobuf::internal::WireFormatLite::ReadUInt32(
              input, &algorithm_));
        _set_bit(4);
        if (input->ExpectTag(48)) goto parse_role;
        break;
      }
      
      // optional .ods.hsmkey.keyrole role = 6 [default = ZSK];
      case 6: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) !=
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          goto handle_uninterpreted;
        }
       parse_role:
        int value;
        DO_(::google::protobuf::internal::WireFormatLite::ReadEnum(input, &value));
        if (ods::hsmkey::keyrole_IsValid(value)) {
          set_role(static_cast< ods::hsmkey::keyrole >(value));
        } else {
          mutable_unknown_fields()->AddVarint(6, value);
        }
        if (input->ExpectTag(58)) goto parse_used_by_zones;
        break;
      }
      
      // repeated string used_by_zones = 7;
      case 7: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) !=
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          goto handle_uninterpreted;
        }
       parse_used_by_zones:
        DO_(::google::protobuf::internal::WireFormatLite::ReadString(
              input, this->add_used_by_zones()));
        ::google::protobuf::internal::WireFormat::VerifyUTF8String(
          this->used_by_zones(0).data(), this->used_by_zones(0).length(),
          ::google::protobuf::internal::WireFormat::PARSE);
        if (input->ExpectTag(58)) goto parse_used_by_zones;
        if (input->ExpectTag(64)) goto parse_inception;
        break;
      }
      
      // optional uint32 inception = 8;
      case 8: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) !=
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          goto handle_uninterpreted;
        }
       parse_inception:
        DO_(::google::protobuf::internal::WireFormatLite::ReadUInt32(
              input, &inception_));
        _set_bit(7);
        if (input->ExpectTag(72)) goto parse_revoke;
        break;
      }
      
      // optional bool revoke = 9 [default = false];
      case 9: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) !=
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          goto handle_uninterpreted;
        }
       parse_revoke:
        DO_(::google::protobuf::internal::WireFormatLite::ReadBool(
              input, &revoke_));
        _set_bit(8);
        if (input->ExpectTag(82)) goto parse_algorithm_name;
        break;
      }
      
      // optional string algorithm_name = 10;
      case 10: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) !=
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          goto handle_uninterpreted;
        }
       parse_algorithm_name:
        DO_(::google::protobuf::internal::WireFormatLite::ReadString(
              input, this->mutable_algorithm_name()));
        ::google::protobuf::internal::WireFormat::VerifyUTF8String(
          this->algorithm_name().data(), this->algorithm_name().length(),
          ::google::protobuf::internal::WireFormat::PARSE);
        if (input->ExpectTag(90)) goto parse_hsm_name;
        break;
      }
      
      // optional string hsm_name = 11;
      case 11: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) !=
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          goto handle_uninterpreted;
        }
       parse_hsm_name:
        DO_(::google::protobuf::internal::WireFormatLite::ReadString(
              input, this->mutable_hsm_name()));
        ::google::protobuf::internal::WireFormat::VerifyUTF8String(
          this->hsm_name().data(), this->hsm_name().length(),
          ::google::protobuf::internal::WireFormat::PARSE);
        if (input->ExpectAtEnd()) return true;
        break;
      }
      
      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void HsmKey::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  ::google::protobuf::uint8* raw_buffer = output->GetDirectBufferForNBytesAndAdvance(_cached_size_);
  if (raw_buffer != NULL) {
    HsmKey::SerializeWithCachedSizesToArray(raw_buffer);
    return;
  }
  
  // required string locator = 1;
  if (_has_bit(0)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->locator().data(), this->locator().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      1, this->locator(), output);
  }
  
  // optional bool candidate_for_sharing = 2 [default = false];
  if (_has_bit(1)) {
    ::google::protobuf::internal::WireFormatLite::WriteBool(2, this->candidate_for_sharing(), output);
  }
  
  // optional uint32 bits = 3 [default = 2048];
  if (_has_bit(2)) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(3, this->bits(), output);
  }
  
  // optional string policy = 4 [default = "default"];
  if (_has_bit(3)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->policy().data(), this->policy().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      4, this->policy(), output);
  }
  
  // optional uint32 algorithm = 5 [default = 1];
  if (_has_bit(4)) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(5, this->algorithm(), output);
  }
  
  // optional .ods.hsmkey.keyrole role = 6 [default = ZSK];
  if (_has_bit(5)) {
    ::google::protobuf::internal::WireFormatLite::WriteEnum(
      6, this->role(), output);
  }
  
  // repeated string used_by_zones = 7;
  for (int i = 0; i < this->used_by_zones_size(); i++) {
  ::google::protobuf::internal::WireFormat::VerifyUTF8String(
    this->used_by_zones(i).data(), this->used_by_zones(i).length(),
    ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      7, this->used_by_zones(i), output);
  }
  
  // optional uint32 inception = 8;
  if (_has_bit(7)) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(8, this->inception(), output);
  }
  
  // optional bool revoke = 9 [default = false];
  if (_has_bit(8)) {
    ::google::protobuf::internal::WireFormatLite::WriteBool(9, this->revoke(), output);
  }
  
  // optional string algorithm_name = 10;
  if (_has_bit(9)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->algorithm_name().data(), this->algorithm_name().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      10, this->algorithm_name(), output);
  }
  
  // optional string hsm_name = 11;
  if (_has_bit(10)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->hsm_name().data(), this->hsm_name().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      11, this->hsm_name(), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* HsmKey::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required string locator = 1;
  if (_has_bit(0)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->locator().data(), this->locator().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        1, this->locator(), target);
  }
  
  // optional bool candidate_for_sharing = 2 [default = false];
  if (_has_bit(1)) {
    target = ::google::protobuf::internal::WireFormatLite::WriteBoolToArray(2, this->candidate_for_sharing(), target);
  }
  
  // optional uint32 bits = 3 [default = 2048];
  if (_has_bit(2)) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt32ToArray(3, this->bits(), target);
  }
  
  // optional string policy = 4 [default = "default"];
  if (_has_bit(3)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->policy().data(), this->policy().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        4, this->policy(), target);
  }
  
  // optional uint32 algorithm = 5 [default = 1];
  if (_has_bit(4)) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt32ToArray(5, this->algorithm(), target);
  }
  
  // optional .ods.hsmkey.keyrole role = 6 [default = ZSK];
  if (_has_bit(5)) {
    target = ::google::protobuf::internal::WireFormatLite::WriteEnumToArray(
      6, this->role(), target);
  }
  
  // repeated string used_by_zones = 7;
  for (int i = 0; i < this->used_by_zones_size(); i++) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->used_by_zones(i).data(), this->used_by_zones(i).length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target = ::google::protobuf::internal::WireFormatLite::
      WriteStringToArray(7, this->used_by_zones(i), target);
  }
  
  // optional uint32 inception = 8;
  if (_has_bit(7)) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt32ToArray(8, this->inception(), target);
  }
  
  // optional bool revoke = 9 [default = false];
  if (_has_bit(8)) {
    target = ::google::protobuf::internal::WireFormatLite::WriteBoolToArray(9, this->revoke(), target);
  }
  
  // optional string algorithm_name = 10;
  if (_has_bit(9)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->algorithm_name().data(), this->algorithm_name().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        10, this->algorithm_name(), target);
  }
  
  // optional string hsm_name = 11;
  if (_has_bit(10)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->hsm_name().data(), this->hsm_name().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        11, this->hsm_name(), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int HsmKey::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required string locator = 1;
    if (has_locator()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->locator());
    }
    
    // optional bool candidate_for_sharing = 2 [default = false];
    if (has_candidate_for_sharing()) {
      total_size += 1 + 1;
    }
    
    // optional uint32 bits = 3 [default = 2048];
    if (has_bits()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt32Size(
          this->bits());
    }
    
    // optional string policy = 4 [default = "default"];
    if (has_policy()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->policy());
    }
    
    // optional uint32 algorithm = 5 [default = 1];
    if (has_algorithm()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt32Size(
          this->algorithm());
    }
    
    // optional .ods.hsmkey.keyrole role = 6 [default = ZSK];
    if (has_role()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::EnumSize(this->role());
    }
    
    // optional uint32 inception = 8;
    if (has_inception()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt32Size(
          this->inception());
    }
    
  }
  if (_has_bits_[8 / 32] & (0xffu << (8 % 32))) {
    // optional bool revoke = 9 [default = false];
    if (has_revoke()) {
      total_size += 1 + 1;
    }
    
    // optional string algorithm_name = 10;
    if (has_algorithm_name()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->algorithm_name());
    }
    
    // optional string hsm_name = 11;
    if (has_hsm_name()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->hsm_name());
    }
    
  }
  // repeated string used_by_zones = 7;
  total_size += 1 * this->used_by_zones_size();
  for (int i = 0; i < this->used_by_zones_size(); i++) {
    total_size += ::google::protobuf::internal::WireFormatLite::StringSize(
      this->used_by_zones(i));
  }
  
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  _cached_size_ = total_size;
  return total_size;
}

void HsmKey::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const HsmKey* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const HsmKey*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void HsmKey::MergeFrom(const HsmKey& from) {
  GOOGLE_CHECK_NE(&from, this);
  used_by_zones_.MergeFrom(from.used_by_zones_);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from._has_bit(0)) {
      set_locator(from.locator());
    }
    if (from._has_bit(1)) {
      set_candidate_for_sharing(from.candidate_for_sharing());
    }
    if (from._has_bit(2)) {
      set_bits(from.bits());
    }
    if (from._has_bit(3)) {
      set_policy(from.policy());
    }
    if (from._has_bit(4)) {
      set_algorithm(from.algorithm());
    }
    if (from._has_bit(5)) {
      set_role(from.role());
    }
    if (from._has_bit(7)) {
      set_inception(from.inception());
    }
  }
  if (from._has_bits_[8 / 32] & (0xffu << (8 % 32))) {
    if (from._has_bit(8)) {
      set_revoke(from.revoke());
    }
    if (from._has_bit(9)) {
      set_algorithm_name(from.algorithm_name());
    }
    if (from._has_bit(10)) {
      set_hsm_name(from.hsm_name());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void HsmKey::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void HsmKey::CopyFrom(const HsmKey& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool HsmKey::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;
  
  return true;
}

void HsmKey::Swap(HsmKey* other) {
  if (other != this) {
    std::swap(locator_, other->locator_);
    std::swap(candidate_for_sharing_, other->candidate_for_sharing_);
    std::swap(bits_, other->bits_);
    std::swap(policy_, other->policy_);
    std::swap(algorithm_, other->algorithm_);
    std::swap(role_, other->role_);
    used_by_zones_.Swap(&other->used_by_zones_);
    std::swap(inception_, other->inception_);
    std::swap(revoke_, other->revoke_);
    std::swap(algorithm_name_, other->algorithm_name_);
    std::swap(hsm_name_, other->hsm_name_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata HsmKey::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = HsmKey_descriptor_;
  metadata.reflection = HsmKey_reflection_;
  return metadata;
}


}  // namespace hsmkey
}  // namespace ods
